#include "tcpserver.h"

#include <iostream>
#include <asio.hpp>

#include "config.h"

int main()
{
    try
    {
        std::shared_ptr<Config> config = std::make_shared<Config>();
        config->load("./config.txt");
        asio::io_context io_context;
        TcpServer server(io_context, config);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
