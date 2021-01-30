#include <consolepp/console.hpp>
#include <gtest/gtest.h>

TEST(console, test)
{
    using namespace consolepp::literals;

    boost::asio::io_context ctx;
    consolepp::console csl(ctx);

    try
    {
        auto const console_size = csl.size();
        std::cout << "console size is " 
                  << console_size.width
                  << "x"
                  << console_size.height
                  << "\n";
    }
    catch (consolepp::invalid_console const &)
    {
        std::cout << "Invalid console\n";
    }

    EXPECT_TRUE(true);
}
