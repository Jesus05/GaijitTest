#include "tcpconnection.h"

#include <iostream>
#include <sstream>

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

    // std::stringstream ss;

    while(1)
    {
        asio::error_code error;
        size_t len = socket_.read_some(asio::buffer(readMessage_), error);
        if (error || len == 0) break;
        if (auto it = std::find(readMessage_.begin(), readMessage_.end(), '\n'); it != readMessage_.end()) {
            it++;/*\n*/
            const size_t writed = std::distance(readMessage_.begin(), it);
            std::cout << "distance:" << writed << std::endl;
            buffer_.write(&readMessage_[0], writed);
            *it = '\0';
            std::string line;
            std::cout << (bool)std::getline(buffer_, line) << std::endl;
            std::cout << "Line:" << line << std::endl;

            std::cout << "distance2:" << std::distance(it, readMessage_.end()) << std::endl;
            std::cout << "it != readMessage_.end():" << (it != readMessage_.end()) << std::endl;

            buffer_.write(&readMessage_[0], len-writed);
        }
        else {
            buffer_.write(&readMessage_[0], len);
        }
        std::cout << "read len:" << len << " e:" << error << std::endl;
        std::cout << &readMessage_[0] << std::endl;
//        std::string line;
//        std::cout << (bool)std::getline(ss, line) << std::endl;
//        std::cout << line << std::endl;
    }

    std::cout << "Exit" << std::endl;

    // asio::async_write(socket_, asio::buffer(message_),
    //                   std::bind(&TcpConnection::handle_write, shared_from_this(),
    //                             asio::placeholders::error,
    //                             asio::placeholders::bytes_transferred));

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
