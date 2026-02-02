#include "tcpserver.h"

using asio::ip::tcp;

TcpServer::TcpServer(asio::io_context &io_context)
    : ioContext_(io_context),
    acceptor_(io_context, tcp::endpoint(tcp::v4(), 13))
{
    startAccept();
}

void TcpServer::startAccept()
{
    tcp_connection::pointer newConnection =
        tcp_connection::create(ioContext_);

    acceptor_.async_accept(newConnection->socket(),
                           std::bind(&TcpServer::handleAccept, this, newConnection,
                                     asio::placeholders::error));
}

void TcpServer::handleAccept(tcp_connection::pointer newConnection, const asio::error_code &error)
{
    if (!error)
    {
        newConnection->start();
    }

    startAccept();
}
