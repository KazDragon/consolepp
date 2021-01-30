#pragma once

#include "consolepp/core.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <utility>

namespace consolepp {

struct invalid_console {};

struct extent
{
    int width;
    int height;    
};

class CONSOLEPP_EXPORT console final
{
public:
    explicit console(boost::asio::io_context &io_context);
    ~console();

    void write(bytes data);

    template <class ReadContinuation>
    void async_read(ReadContinuation &&read_continuation)
    {
        stream_.async_read_some(
            boost::asio::buffer(read_buffer_, read_buffer_.size()),
            [this, 
             read_continuation = 
                 std::forward<ReadContinuation>(read_continuation)](
                boost::system::error_code const &ec,
                std::size_t bytes_transferred)
            {
                read_continuation(
                    bytes{read_buffer_.data(), bytes_transferred});
            });
    }

    extent size() const;

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;

    boost::asio::posix::stream_descriptor stream_;
    byte_storage read_buffer_;
};

}
