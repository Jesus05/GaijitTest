#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>
#include <sstream>
#include <optional>

#include <asio.hpp>

#include "values.h"

class TcpConnection
    : public std::enable_shared_from_this<TcpConnection>
{
public:
    typedef std::shared_ptr<TcpConnection> pointer;

    static pointer create(asio::io_context& io_context, Values::pointer values);

    asio::ip::tcp::socket& socket();

    void start();

private:
    TcpConnection(asio::io_context& io_context, Values::pointer values);

    struct Command{
        enum Type { GET, SET };
        Type command;
        std::string key;
        std::string value;
    };

    bool accumulateBuffer();
    std::optional<Command> parseCommand(std::string_view line);

    void executeGet(const std::string &key);
    void executeSet(const std::string &key, const std::string &value);

    asio::ip::tcp::socket socket_;
    std::stringstream buffer_;
    Values::pointer values_;
};

#endif // TCP_CONNECTION_H
