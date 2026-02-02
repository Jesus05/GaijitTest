//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

std::string data;

void handler(const asio::error_code& e, std::size_t n)
{
    if (!e)
    {
        std::string line = data.substr(0, n);
        data.erase(0, n);
    }
}

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    static pointer create(asio::io_context& io_context)
    {
        return pointer(new tcp_connection(io_context));
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        message_ = make_daytime_string();

        asio::error_code error;

        size_t len = socket_.read_some(asio::buffer(readMessage_), error);
        std::cout << "read len:" << len << " e:" << error << std::endl;
        std::cout << &readMessage_[0] << std::endl;
        std::cout << message_ << std::endl;


        asio::async_write(socket_, asio::buffer(message_),
                          std::bind(&tcp_connection::handle_write, shared_from_this(),
                                    asio::placeholders::error,
                                    asio::placeholders::bytes_transferred));

//        asio::async_read_until(socket_, data, '\n', handler);

        // asio::async_read_until(socket_, asio::buffer(readMessage_), "\n",
        //                        std::bind(&tcp_connection::handle_read, shared_from_this(),
        //                                                                             asio::placeholders::error,
        //                                                                             asio::placeholders::bytes_transferred) );

        // asio::async_read(socket_, asio::buffer(readMessage_),
        //                         std::bind(&tcp_connection::handle_read, shared_from_this(),
        //                                    asio::placeholders::error,
        //                                    asio::placeholders::bytes_transferred));

    }

private:
    tcp_connection(asio::io_context& io_context)
        : socket_(io_context)
    {
    }

    void handle_write(const asio::error_code& error,
                      size_t bytes_transferred)
    {
        std::cout << "writed:" << bytes_transferred << " e:" << error << std::endl;
        start();
    }

    void handle_read(const asio::error_code& error,
                     size_t bytes_transferred)
    {
        std::cout << "readed:" << bytes_transferred << " e:" << error << std::endl;
        readMessage_[bytes_transferred] = '\0';
        std::cout << &readMessage_[0] << std::endl;

        if (bytes_transferred == 0)
        {
            asio::async_read(socket_, asio::buffer(readMessage_),
                             std::bind(&tcp_connection::handle_read, shared_from_this(),
                                       asio::placeholders::error,
                                       asio::placeholders::bytes_transferred));
        }
        else
        {
            asio::async_write(socket_, asio::buffer(message_),
                          std::bind(&tcp_connection::handle_write, shared_from_this(),
                                    asio::placeholders::error,
                                    asio::placeholders::bytes_transferred));
        }
    }

    tcp::socket socket_;
    std::string message_;
    std::array<char, 128> readMessage_;
};

class tcp_server
{
public:
    tcp_server(asio::io_context& io_context)
        : io_context_(io_context),
        acceptor_(io_context, tcp::endpoint(tcp::v4(), 13))
    {
        start_accept();
    }

private:
    void start_accept()
    {
        tcp_connection::pointer new_connection =
            tcp_connection::create(io_context_);

        acceptor_.async_accept(new_connection->socket(),
                               std::bind(&tcp_server::handle_accept, this, new_connection,
                                         asio::placeholders::error));
    }

    void handle_accept(tcp_connection::pointer new_connection,
                       const asio::error_code& error)
    {
        if (!error)
        {
            new_connection->start();
        }

        start_accept();
    }

    asio::io_context& io_context_;
    tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        asio::io_context io_context;
        tcp_server server(io_context);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
