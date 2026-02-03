#include "tcpconnection.h"

#include <iostream>
#include <sstream>

using asio::ip::tcp;

std::string_view trim(std::string_view str) {
    auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string_view::npos) {
        return "";
    }

    auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

TcpConnection::pointer TcpConnection::create(asio::io_context &io_context, Values::pointer values, Stats::pointer stats) {
    return pointer(new TcpConnection(io_context, values, stats));
}

tcp::socket &TcpConnection::socket() {
    return socket_;
}

void TcpConnection::start() {
    std::cout << "open session:" << (void*)this << std::endl;

    while(1) {
        if (!accumulateBuffer()) break;
    }

    std::cout << "close session:" << (void*)this << std::endl;
}

TcpConnection::TcpConnection(asio::io_context &io_context, Values::pointer values, Stats::pointer stats)
    : socket_(io_context),
    values_(values),
    stats_(stats),
    bufferUse_(0){
}

bool TcpConnection::readUntil(const asio::mutable_buffer& buffer, char delimiter, std::string& result, asio::error_code& error, size_t &totalRead) {
    result.clear();

    size_t bytesRead = totalRead;

    while (true) {
        const char* data = static_cast<const char*>(buffer.data());
        for (size_t i = totalRead - bytesRead; i < totalRead; ++i) {
            if (data[i] == delimiter) {
                result.assign(data, i);

                size_t remaining = totalRead - i - 1;
                if (remaining > 0) {
                    std::memmove(buffer.data(), data + i + 1, remaining);
                }
                totalRead = remaining;

                return true;
            }
        }

        bytesRead = socket_.read_some(buffer + totalRead, error);

        if (error || bytesRead == 0) {
            return false;
        }

        totalRead += bytesRead;

        if (totalRead >= buffer.size()) {
            error = asio::error::message_size;
            return false;
        }
    }
}

bool TcpConnection::accumulateBuffer()
{
    asio::error_code error;
    std::string line;

    if (!readUntil(asio::buffer(buffer_), '\n', line, error, bufferUse_)) {
        std::cerr << "Read error: " << error.message() << std::endl;
        return false;
    }

    const auto command = parseCommand(line);
    if (command){
        const auto &cmd = command.value();
        switch(cmd.command) {
            case Command::GET: executeGet(cmd.key); break;
            case Command::SET: executeSet(cmd.key, cmd.value); break;
        }
    }

    return true;
}

std::optional<TcpConnection::Command> TcpConnection::parseCommand(std::string_view input) {
    input = trim(input);

    if (input.empty()) {
        return std::nullopt;
    }

    if (input[0] != '$') {
        return std::nullopt;
    }

    input.remove_prefix(1);
    input = trim(input);

    if (input.size() >= 3 &&
        (input[0] == 'g' || input[0] == 'G') &&
        (input[1] == 'e' || input[1] == 'E') &&
        (input[2] == 't' || input[2] == 'T')) {

        input.remove_prefix(3);

        input = trim(input);

        if (input.empty()) {
            return std::nullopt;
        }

        std::string key = std::string(input);

        key.erase(std::find_if(key.rbegin(), key.rend(),
                                  [](unsigned char ch) { return !std::isspace(ch); }).base(),
                     key.end());

        return Command{Command::GET, key, ""};
    }

    if (input.size() >= 3 &&
        (input[0] == 's' || input[0] == 'S') &&
        (input[1] == 'e' || input[1] == 'E') &&
        (input[2] == 't' || input[2] == 'T')) {

        input.remove_prefix(3);

        input = trim(input);

        if (input.empty()) {
            return std::nullopt;
        }

        size_t eq_pos = input.find('=');
        if (eq_pos == std::string_view::npos) {
            return std::nullopt; // Нет знака =
        }

        std::string_view key = input.substr(0, eq_pos);
        key = trim(key);

        if (key.empty()) {
            return std::nullopt;
        }

        std::string_view value = input.substr(eq_pos + 1);
        value = trim(value);

        return Command{Command::SET, std::string(key), std::string(value)};
    }

    return std::nullopt;
}

void TcpConnection::executeGet(const std::string &key) {
    if (stats_) {
        stats_->recordRead(key);
    }
    const auto value = values_->get(key);
    std::string response;
    if (value) {
        response = "OK " + key + "=" + *value + "\n";
    } else {
        response = "ERROR KEY_NOT_FOUND\n";
    }
    if (stats_) {
        auto keyStats = stats_->getKeyStats(key);
        response += "reads=" + std::to_string(keyStats.reads) + "\n";
        response += "writes=" + std::to_string(keyStats.writes) + "\n";
    }

    sendResponse(response);
}

void TcpConnection::executeSet(const std::string &key, const std::string &value) {
    if (stats_) {
        stats_->recordWrite(key);
    }
    values_->set(key, value);
    std::string response = "OK\n";
    if (stats_) {
        auto keyStats = stats_->getKeyStats(key);
        response += "reads=" + std::to_string(keyStats.reads) + "\n";
        response += "writes=" + std::to_string(keyStats.writes) + "\n";
    }

    sendResponse(response);
}

void TcpConnection::sendResponse(const std::string& response) {
    asio::error_code error;
    asio::write(socket_, asio::buffer(response), error);

    if (error) {
        std::cerr << "Write error: " << error.message() << std::endl;
    }
}
