#pragma once

#include "consolepp/core.hpp"
#include <boost/asio.hpp>
#include <memory>

namespace consolepp {

struct invalid_console {};

class CONSOLEPP_EXPORT console final
{
public:
    explicit console(boost::asio::io_context &io_context);
    ~console();

    //void write()

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

}
