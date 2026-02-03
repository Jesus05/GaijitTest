#pragma once
#include <asio.hpp>

#include "tcpconnection.h"
#include "values.h"
#include "stats.h"

class TcpServer
{
public:
    TcpServer(asio::io_context& io_context, Values::pointer values, Stats::pointer stats);

private:
    void startAccept();

    void handleAccept(TcpConnection::pointer newConnection,
                       const asio::error_code& error);

    asio::io_context& ioContext_;
    asio::ip::tcp::acceptor acceptor_;
    Values::pointer values_;
    Stats::pointer stats_;
};
