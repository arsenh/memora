#include <iostream>
#include <boost/asio.hpp>

#include "memora/server/server.hpp"

int main()
{
    try
    {
        boost::asio::io_context io_context;
        memora::Server server(io_context, 8080);
        server.run();
        io_context.run();
    } catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
