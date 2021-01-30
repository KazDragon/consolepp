#include <consolepp/console.hpp>
#include <gtest/gtest.h>

TEST(dummy, test)
{
    boost::asio::io_context ctx;
    consolepp::console csl(ctx);
    EXPECT_TRUE(true);
}
