#include "consolepp/console.hpp"
#include <boost/make_unique.hpp>
#include <termios.h>
#include <unistd.h>

namespace consolepp {

namespace {

constexpr int default_read_buffer_size = 1024;

termios set_console_mode(int descriptor)
{
    termios old_attributes;

    if (::tcgetattr(descriptor, &old_attributes) == 0)
    {
        auto const new_attributes = [&]{
            auto new_attributes = old_attributes;
            new_attributes.c_lflag &= ~(ICANON | ECHO);
            return new_attributes;
        }();

        if (::tcsetattr(descriptor, TCSANOW, &new_attributes) != 0)
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

void restore_console_mode(int descriptor, termios const &attributes)
{
    ::tcsetattr(descriptor, TCSANOW, &attributes);
}

}

struct console::impl
{
    impl(boost::asio::io_context &ctx)
      : io_context_(ctx),
        old_attributes_(set_console_mode(STDIN_FILENO)),
        console_descriptor_(::dup(STDIN_FILENO))
    {
    }

    ~impl()
    {
        restore_console_mode(STDIN_FILENO, old_attributes_);
    }

    boost::asio::io_context &io_context_;
    termios old_attributes_;
    int console_descriptor_;
};

console::console(boost::asio::io_context &ctx)
  : pimpl_(boost::make_unique<impl>(ctx)),
    stream_(ctx, pimpl_->console_descriptor_),
    read_buffer_(default_read_buffer_size, '\0')
    
{
}

console::~console() = default;

void console::write(bytes data)
{
    stream_.write_some(boost::asio::const_buffer{data.begin(), data.size()});
}

}
