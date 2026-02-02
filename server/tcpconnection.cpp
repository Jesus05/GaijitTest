#include "tcpconnection.h"

#include <iostream>

using asio::ip::tcp;

TcpConnection::pointer TcpConnection::create(asio::io_context &io_context)
{
    return pointer(new TcpConnection(io_context));
}

tcp::socket &TcpConnection::socket()
{
    return socket_;
}

void TcpConnection::start()
{
    message_ = make_daytime_string();

    asio::error_code error;

    size_t len = socket_.read_some(asio::buffer(readMessage_), error);
    std::cout << "read len:" << len << " e:" << error << std::endl;
    std::cout << &readMessage_[0] << std::endl;
    std::cout << message_ << std::endl;

    asio::async_write(socket_, asio::buffer(message_),
                      std::bind(&TcpConnection::handle_write, shared_from_this(),
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

TcpConnection::TcpConnection(asio::io_context &io_context)
    : socket_(io_context)
{
}

void TcpConnection::handle_write(const asio::error_code &error, size_t bytes_transferred)
{
    std::cout << "writed:" << bytes_transferred << " e:" << error << std::endl;
    start();
}

void TcpConnection::handle_read(const asio::error_code &error, size_t bytes_transferred)
{
    std::cout << "readed:" << bytes_transferred << " e:" << error << std::endl;
    readMessage_[bytes_transferred] = '\0';
    std::cout << &readMessage_[0] << std::endl;

    if (bytes_transferred == 0)
    {
        asio::async_read(socket_, asio::buffer(readMessage_),
                         std::bind(&TcpConnection::handle_read, shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));
    }
    else
    {
        asio::async_write(socket_, asio::buffer(message_),
                          std::bind(&TcpConnection::handle_write, shared_from_this(),
                                    asio::placeholders::error,
                                    asio::placeholders::bytes_transferred));
    }
}

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}
