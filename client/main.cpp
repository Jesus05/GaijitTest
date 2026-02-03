//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <array>
#include <iostream>
#include <asio.hpp>
#include <cstdlib>

using asio::ip::tcp;

std::vector<std::string> predefinedKeys = {
    "server_port",
    "server_host",
    "max_connections",
    "thread_pool_size",
    "log_level",
    "log_file",
    "timeout",
    "keep_alive",
    "db_host",
    "db_port",
    "db_name",
    "db_user",
    "db_password",
    "db_pool_size",
    "db_timeout",
    "cache_size",
    "cache_ttl",
    "cache_type",
    "redis_host",
    "redis_port",
    "ssl_enabled",
    "ssl_cert",
    "ssl_key",
    "auth_token",
    "metrics_port",
    "health_check",
    "statistics_interval",
    "environment",
    "version",
    "debug_mode",
    "feature_flag",
    "maintenance",
    "backup_path",
    "temp_directory",
    "user_id",
    "user_name",
    "email",
    "language",
    "timezone",
    "currency",
    "app_name",
    "app_version",
    "api_version",
    "default_route",
    "error_page",
    "proxy_host",
    "proxy_port",
    "dns_server",
    "backup_encryption",
    "auto_update",
    "update_channel",
    "update_check_interval",
    "test_mode",
    "mock_enabled",
    "fixture_path",
    "coverage_threshold"
};

std::vector<std::string> predefinedValues = {
    "true",
    "false",
    "12345",
    "8080",
    "localhost",
    "production",
    "development",
    "testing",
    "admin",
    "user",
    "qwerty123",
    "abc123",
    "test_value",
    "sample_data",
    "127.0.0.1",
    "192.168.1.1",
    "10.0.0.1",
    "/var/log",
    "/tmp",
    "http://localhost:8080",
    "INFO",
    "WARN",
    "ERROR",
    "DEBUG",
    "red",
    "green",
    "blue",
    "yellow",
    "2024-01-15",
    "12:30:00",
    "3600",
    "default",
    "none",
    "auto",
    "manual"
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }

    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints =
      resolver.resolve(argv[1], "12345");

    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    srand(static_cast<int>(time(NULL)));

    for (int i = 0; i < 10000; i++) {
        std::string command;
        if (rand()%100) {
            command += "$get";
            command += predefinedKeys[rand() % predefinedKeys.size()];
            command += "\n";
        } else {
            command += "$set";
            command += predefinedKeys[rand() % predefinedKeys.size()];
            command += "=";
            command += predefinedValues[rand() % predefinedValues.size()];
            command += "\n";
        }
        asio::error_code error;
        socket.write_some(asio::buffer(command), error);
        if (error)
            throw asio::system_error(error);

        std::array<char, 4096> buf;
        const size_t len = socket.read_some(asio::buffer(buf), error);

        if (error == asio::error::eof)
            break; // Connection closed cleanly by peer.
        else if (error)
            throw asio::system_error(error); // Some other error.

        std::cout.write(buf.data(), len);
    }

    socket.close();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
