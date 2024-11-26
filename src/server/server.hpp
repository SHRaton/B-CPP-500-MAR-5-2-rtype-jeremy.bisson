#pragma once
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <map>
#include <chrono>

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
}

class Server {
public:
    Server(uint16_t port);
    void start();

private:
    static const int TIMEOUT_MS = 5000;

    struct ClientInfo {
        std::string name;
        std::chrono::steady_clock::time_point last_seen;
    };

    void receive_messages();
    void check_timeouts();

    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket socket_;
    std::map<boost::asio::ip::udp::endpoint, ClientInfo> clients_;
    bool running_;
    std::thread receive_thread_;
};