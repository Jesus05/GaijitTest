#pragma once
#include <asio.hpp>

#include "tcp_connection.h"

class TcpServer
{
public:
    TcpServer(asio::io_context& io_context);

private:
    void startAccept();

    void handleAccept(tcp_connection::pointer newConnection,
                       const asio::error_code& error);

    asio::io_context& ioContext_;
    asio::ip::tcp::acceptor acceptor_;
};
