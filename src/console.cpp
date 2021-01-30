#include "consolepp/console.hpp"
#include <boost/make_unique.hpp>
#include <termios.h>
#include <unistd.h>

namespace consolepp {

namespace {

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
    tcsetattr(descriptor, TCSANOW, &attributes);
}

}

struct console::impl
{
    impl(boost::asio::io_context &ctx)
      : io_context_(ctx),
        console_descriptor_(::dup(STDIN_FILENO)),
        old_attributes_(set_console_mode(console_descriptor_)),
        stream_(io_context_, console_descriptor_)
    {
    }

    ~impl()
    {
        restore_console_mode(console_descriptor_, old_attributes_);
    }

    boost::asio::io_context &io_context_;
    int console_descriptor_;
    termios old_attributes_;
    boost::asio::posix::stream_descriptor stream_;

};

console::console(boost::asio::io_context &ctx)
  : pimpl_(boost::make_unique<impl>(ctx))
{
}

console::~console() = default;

}
