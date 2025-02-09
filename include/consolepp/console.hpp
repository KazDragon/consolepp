#pragma once

#include "consolepp/core.hpp"

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <memory>
#include <utility>

namespace consolepp {

//* =========================================================================
/// An exception that is thrown whenever the console cannot perform a
/// function due to a failure in an OS primitive.
//* =========================================================================
struct invalid_console
{
};

//* =========================================================================
/// A structure that encapsulates the size of a console.
//* =========================================================================
struct CONSOLEPP_EXPORT extent final
{
    /// A horizontal width.
    int width;

    /// A vertical height.
    int height;
};

//* =========================================================================
/// A class that represents a console.
///
/// \par Usage
/// Construct a console by passing an io_context object.  All reading and
/// writing to the console will be performed in this context, along with
/// any signal handling required for e.g. console size change notifications,
/// and will thus occur only on threads where the io_context object is
/// running.
/// \code
/// consolepp::console csl{my_io_context};
/// \endcode
///
/// \par
/// You can write to the console by passing in any sequence of bytes.  A
/// UDL is supplied for easy conversion of string literals.
/// \code
/// using namespace consolepp::literals;
/// std::vector<consolepp::byte> hello = { 74, 101, 108, 108, 111, 44, 32 };
/// csl.write(hello);
/// csl.write("world!\n"_cb);
/// \endcode
///
/// \par
/// Reads are performed asynchronously, and require a function to be passed
/// that is called back when the read completes.
/// \code
/// std::vector<consolepp::byte_storage> input;
/// csl.async_read(
///     [&input](consolepp::bytes data)
///     {
///         input.insert(input.end(), data.begin(), data.end());
///     });
/// \endcode
///
/// \par
/// The console can also be queried about its size.  You can also be notified
/// whenever the console size changes by registering with the on_size_changed
/// signal.
/// \code
/// auto size = csl.size();
/// csl.on_size_changed.connect(
///     [&]()
///     {
///         size = csl.size();
///     });
/// \endcode
///
/// \par Channel
/// The console aligns with the channel concept used across other related
/// libraries, including Server++, Terminal++ and Telnet++.
//* =========================================================================
class CONSOLEPP_EXPORT console final
{
public:
    //* =====================================================================
    /// Constructor
    //* =====================================================================
    explicit console(boost::asio::io_context &io_context);

    //* =====================================================================
    /// Copy Constructor
    //* =====================================================================
    console(console const &) = delete;

    //* =====================================================================
    /// Destructor
    //* =====================================================================
    ~console();

    //* =====================================================================
    /// Copy Assignment
    //* =====================================================================
    console &operator=(console const &) = delete;

    //* =====================================================================
    /// Write data to the console.
    //* =====================================================================
    void write(bytes data);

    //* =====================================================================
    /// \brief Read from the console, with the data being forwarded to the
    /// passed continuation.
    //* =====================================================================
    template <class ReadContinuation>
    void async_read(ReadContinuation &&read_continuation)
    {
        stream_.async_read_some(
            boost::asio::buffer(read_buffer_, read_buffer_.size()),
            [this,
             read_continuation =
                 std::forward<ReadContinuation>(read_continuation)](
                boost::system::error_code const &ec,
                std::size_t bytes_transferred) {
                read_continuation(
                    bytes{read_buffer_.data(), bytes_transferred});
            });
    }

    //* =====================================================================
    /// \brief Returns whether the console is alive or not.
    //* =====================================================================
    [[nodiscard]] bool is_alive() const;

    //* =====================================================================
    /// \brief Closes the console.
    //* =====================================================================
    void close();

    //* =====================================================================
    /// Returns the size of the console.
    //* =====================================================================
    [[nodiscard]] extent size() const;

    //* =====================================================================
    /// A callback for when the console size changes.
    //* =====================================================================
    boost::signals2::signal<void()> on_size_changed;  // NOLINT

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;

    boost::asio::posix::stream_descriptor stream_;
    byte_storage read_buffer_;
};

}  // namespace consolepp
