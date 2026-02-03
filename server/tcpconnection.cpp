#include "tcpconnection.h"

#include <iostream>
#include <sstream>

using asio::ip::tcp;

TcpConnection::pointer TcpConnection::create(asio::io_context &io_context, Values::pointer values)
{
    return pointer(new TcpConnection(io_context, values));
}

tcp::socket &TcpConnection::socket()
{
    return socket_;
}

void TcpConnection::start()
{
    while(1)
    {
        if (!accumulateBuffer()) break;
    }

    std::cout << "Exit" << std::endl;
}

TcpConnection::TcpConnection(asio::io_context &io_context, Values::pointer values)
    : socket_(io_context),
    values_(values)
{
}

bool TcpConnection::accumulateBuffer()
{
    std::array<char, 128> readMessage;
    asio::error_code error;
    size_t len = socket_.read_some(asio::buffer(readMessage), error);
    if (error || len == 0) return false;
    if (auto it = std::find(readMessage.begin(), readMessage.end(), '\n'); it != readMessage.end()) {
        it++;/*\n*/
        const size_t writed = std::distance(readMessage.begin(), it);
        std::cout << "distance:" << writed << std::endl;
        buffer_.write(&readMessage[0], writed);
        *it = '\0';
        std::string line;
        std::cout << (bool)std::getline(buffer_, line) << std::endl;
        std::cout << "Line:" << line << std::endl;

        const auto command = parseCommand(line);
        if (command) executeCommand(command.value());

        std::cout << "distance2:" << std::distance(it, readMessage.end()) << std::endl;
        std::cout << "it != readMessage_.end():" << (it != readMessage.end()) << std::endl;

        buffer_.write(&readMessage[0], len-writed);
    }
    else {
        buffer_.write(&readMessage[0], len);
    }
    std::cout << "read len:" << len << " e:" << error << std::endl;
    std::cout << &readMessage[0] << std::endl;
    return true;
}

std::optional<std::string> TcpConnection::parseCommand(std::string_view line) {

}

void TcpConnection::executeCommand(std::string_view commandString) {
    const std::string_view command = commandString.substr(0, 4);

    if (command == "$get") {
        const std::string_view key = commandString.substr(6);
        const auto value = values_->get(key);
        //TODO
    }
    if (command == "$set") {
        const size_t eq = commandString.find('=');
        const std::string_view key = commandString.substr(6, eq-6);
        const std::string_view value = commandString.substr(eq+1);
        values_->set(key, value);
        //TODO
    }
}
