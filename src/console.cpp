#include "consolepp/console.hpp"

#include <boost/asio/error.hpp>
#include <boost/make_unique.hpp>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace consolepp {

namespace {

constexpr int default_read_buffer_size = 1024;

// ==========================================================================
// SET_CONSOLE_MODE
// ==========================================================================
termios set_console_mode(int descriptor)
{
    termios old_attributes;

    if (::tcgetattr(descriptor, &old_attributes) == 0)
    {
        if (auto const new_attributes =
                [&] {
                    auto new_attrs = old_attributes;
                    new_attrs.c_lflag &=
                        static_cast<tcflag_t>(~(ICANON | ECHO));
                    return new_attrs;
                }();
            ::tcsetattr(descriptor, TCSANOW, &new_attributes) != 0)
        {
            throw invalid_console();
        }
    }
    else
    {
        throw invalid_console();
    }

    return old_attributes;
}

// ==========================================================================
// RESTORE_CONSOLE_MODE
// ==========================================================================
void restore_console_mode(int descriptor, termios const &attributes)
{
    ::tcsetattr(descriptor, TCSANOW, &attributes);
}

// ==========================================================================
// GET_CONSOLE_SIZE
// ==========================================================================
extent get_console_size(int descriptor)
{
    auto const window_size = [descriptor] {
        if (winsize ws; ioctl(descriptor, TIOCGWINSZ, &ws) < 0)
        {
            throw invalid_console();
        }
        else
        {
            return ws;
        }
    }();

    return {window_size.ws_col, window_size.ws_row};
}

// ==========================================================================
// WRITE_ALL
// ==========================================================================
void write_all(boost::asio::posix::stream_descriptor &stream, bytes const &data)
{
    auto const *position = data.data();
    auto remaining = data.size();

    while (remaining != 0)
    {
        auto error = boost::system::error_code{};
        auto const bytes_written = stream.write_some(
            boost::asio::const_buffer{position, remaining}, error);

        if (error == boost::asio::error::interrupted)
        {
            continue;
        }

        if (error)
        {
            throw boost::system::system_error{error, "write_some"};
        }

        position += bytes_written;
        remaining -= bytes_written;
    }
}

}  // namespace

// ==========================================================================
// CONSOLE::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct console::impl
{
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl(console &self, boost::asio::io_context &ctx)
      : self_(self),
        io_context_{ctx},
        signals_{io_context_, SIGWINCH},
        old_attributes_{set_console_mode(STDIN_FILENO)},
        console_descriptor_{::dup(STDIN_FILENO)}
    {
        await_console_size_change();
    }

    // ======================================================================
    // COPY CONSTRUCTOR
    // ======================================================================
    impl(impl const &) = delete;

    // ======================================================================
    // DESTRUCTOR
    // ======================================================================
    ~impl()
    {
        signals_.cancel();
        restore_console_mode(STDIN_FILENO, old_attributes_);
    }

    // ======================================================================
    // COPY ASSIGNMENT
    // ======================================================================
    impl &operator=(impl const &) = delete;

    console &self_;                        // NOLINT
    boost::asio::io_context &io_context_;  // NOLINT
    boost::asio::signal_set signals_;      // NOLINT
    termios old_attributes_;               // NOLINT
    int console_descriptor_;               // NOLINT

private:
    // ======================================================================
    // AWAIT_CONSOLE_SIZE_CHANGE
    // ======================================================================
    void await_console_size_change()
    {
        signals_.async_wait([this](
                                boost::system::error_code const & /*ec*/,
                                int /*signal_number*/) {
            self_.on_size_changed();
            await_console_size_change();
        });
    }
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
console::console(boost::asio::io_context &io_context)
  : pimpl_(boost::make_unique<impl>(*this, io_context)),
    stream_(io_context, pimpl_->console_descriptor_),
    read_buffer_(default_read_buffer_size, '\0')

{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
console::~console() = default;

// ==========================================================================
// WRITE
// ==========================================================================
void console::write(bytes data)
{
    if (!data.empty())
    {
        write_all(stream_, data);
    }
}

// ==========================================================================
// IS_ALIVE
// ==========================================================================
bool console::is_alive() const
{
    return stream_.is_open();
}

// ==========================================================================
// CLOSE
// ==========================================================================
void console::close()
{
    stream_.close();
}

// ==========================================================================
// SIZE
// ==========================================================================
extent console::size() const
{
    return get_console_size(STDIN_FILENO);
}

}  // namespace consolepp
