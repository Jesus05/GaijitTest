//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "tcp_connection.h"
#include "tcpserver.h"

#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

// std::string data;

// void handler(const asio::error_code& e, std::size_t n)
// {
//     if (!e)
//     {
//         std::string line = data.substr(0, n);
//         data.erase(0, n);
//     }
// }



int main()
{
    try
    {
        asio::io_context io_context;
        TcpServer server(io_context);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
