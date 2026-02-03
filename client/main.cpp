//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <array>
#include <iostream>
#include <asio.hpp>
#include <thread>

using asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }

    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints =
      resolver.resolve(argv[1], "daytime");

    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    int i = 0;

    for (const auto sym : std::string("SomeMessageToServer\n$set test=ghf hygd\n"))
    {
        i++;
        asio::error_code error;
        std::array<char, 1> buf;
        buf[0] = sym;
        socket.write_some(asio::buffer(buf), error);
        if (error) break;
        if (i%2) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    i = 0;

    for (;;)
    {
      std::array<char, 128> buf;
      asio::error_code error;

      size_t len = 0;

      while (!len) {
        len = socket.write_some(asio::buffer(std::string("SomeMessageToServer") + std::to_string(i++) + "\n"), error);
          std::cout << "write len:" << len << " e:" << error << std::endl;
      }

      len = socket.write_some(asio::buffer(std::string("$get test\n")), error);

      len = socket.read_some(asio::buffer(buf), error);

      if (error == asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw asio::system_error(error); // Some other error.

      std::cout.write(buf.data(), len);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
