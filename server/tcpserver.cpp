#include "tcpserver.h"

using asio::ip::tcp;

TcpServer::TcpServer(asio::io_context &io_context, Values::pointer values, Stats::pointer stats)
    : ioContext_(io_context),
    acceptor_(io_context, tcp::endpoint(tcp::v4(), 12345)),
    values_(values),
    stats_(stats) {
    startAccept();
}

void TcpServer::startAccept() {
    TcpConnection::pointer newConnection =
        TcpConnection::create(ioContext_, values_, stats_);

    acceptor_.async_accept(newConnection->socket(),
                           std::bind(&TcpServer::handleAccept, this, newConnection,
                                     asio::placeholders::error));
}

void TcpServer::handleAccept(TcpConnection::pointer newConnection, const asio::error_code &error) {
    if (!error) {
        std::thread([newConnection](){
            newConnection->start();
        }).detach();
    }

    startAccept();
}
