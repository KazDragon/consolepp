#include <consolepp/console.hpp>
#include <gtest/gtest.h>

TEST(dummy, test)
{
    using namespace consolepp::literals;

    boost::asio::io_context ctx;
    consolepp::console csl(ctx);
    csl.write("Hello, world!\n"_cb);
    EXPECT_TRUE(true);
}
