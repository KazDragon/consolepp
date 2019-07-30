#include "consolepp/console.hpp"
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/make_unique.hpp>
#include <cstdlib>
#include <utility>

#include <signal.h>
#include <sys/ioctl.h>

namespace consolepp {

namespace {

// This is necessary to be able to get back to the impl from the signal 
// callback, since there is no hook for user data.  The consequence of this
// is that it makes no sense to have more than one console object at a time.
console::impl *pimpl = nullptr;

}

struct console::impl
{
    impl(
        boost::asio::io_context &io_context, 
        std::function<void (std::uint16_t, std::uint16_t)> on_size)
      : io_context_(io_context),
        io_strand_(io_context_),
        on_size_(std::move(on_size)),
        input_(io_context_, ::dup(fileno(stdin))),
        output_(io_context_, ::dup(fileno(stdout)))
    {
        consolepp::pimpl = this;
        announce_window_size();
        set_up_window_size_change_handler();
    }

    ~impl()
    {
        restore_old_window_size_change_handler();
        consolepp::pimpl = nullptr;
    }

    void write(bytes data)
    {
        boost::system::error_code ec;
        boost::asio::write(output_, boost::asio::const_buffer(data.data(), data.size()), ec);
    }

private:
    static void window_size_change_handler_thunk(int sig)
    {
        consolepp::pimpl->window_size_change_handler(sig);
    }

    void window_size_change_handler(int sig)
    {
        io_strand_.post([this](){ announce_window_size(); });
    }

    void announce_window_size()
    {
        winsize ws;
        ioctl(fileno(stdin), TIOCGWINSZ, &ws);
        io_strand_.post([ws, this](){ on_size_(ws.ws_col, ws.ws_row); });
    }

    void set_up_window_size_change_handler()
    {
        struct sigaction signal_action;
        signal_action.sa_flags = 0;
        signal_action.sa_handler = &impl::window_size_change_handler_thunk;
        sigaction(SIGWINCH, &signal_action, &old_signal_handler_);
    }

    void restore_old_window_size_change_handler()
    {
        sigaction(SIGWINCH, &old_signal_handler_, nullptr);
    }

    boost::asio::io_context &io_context_;
    boost::asio::io_context::strand io_strand_;
    std::function<void (std::uint16_t, std::uint16_t)> on_size_;
    struct sigaction old_signal_handler_;
    boost::asio::posix::stream_descriptor input_;
    boost::asio::posix::stream_descriptor output_;
};

console::console(
    boost::asio::io_context &io_context, 
    std::function<void (std::uint16_t, std::uint16_t)> on_size)
  : pimpl_(boost::make_unique<impl>(std::ref(io_context), std::move(on_size)))
{
}

console::~console()
{

}

void console::write(bytes data)
{
    pimpl_->write(data);
}

}
