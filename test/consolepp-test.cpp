#include <consolepp/console.hpp>
#include <gtest/gtest.h>

TEST(console, test)
{
    using namespace consolepp::literals;  // NOLINT

    boost::asio::io_context ctx;

    try
    {
        consolepp::console csl(ctx);

        auto const console_size = csl.size();
        std::cout << "console size is " << console_size.width << "x"
                  << console_size.height << "\n";
    }
    catch (consolepp::invalid_console const &)
    {
        std::cout << "Invalid console\n";
    }

    EXPECT_TRUE(true);
}
