#pragma once

#include "consolepp/core.hpp"
#include <boost/asio/io_context.hpp>
#include <functional>
#include <memory>


namespace consolepp {

class console
{
public:
    console(
        boost::asio::io_context &io_context,
        std::function<void (std::uint16_t, std::uint16_t)> on_size);
    ~console();

    void write(bytes data);
    struct impl;

private:
    std::unique_ptr<impl> pimpl_;
};

}
