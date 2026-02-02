#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>

#include <asio.hpp>

std::string make_daytime_string();

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    static pointer create(asio::io_context& io_context);

    asio::ip::tcp::socket& socket();

    void start();

private:
    tcp_connection(asio::io_context& io_context);

    void handle_write(const asio::error_code& error,
                      size_t bytes_transferred);

    void handle_read(const asio::error_code& error,
                     size_t bytes_transferred);

    asio::ip::tcp::socket socket_;
    std::string message_;
    std::array<char, 128> readMessage_;
};

#endif // TCP_CONNECTION_H
