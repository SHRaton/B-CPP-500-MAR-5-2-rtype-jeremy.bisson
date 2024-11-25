#pragma once
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <map>

class ChatServer {
public:
    ChatServer(uint16_t port);
    void start();

private:
    void receive_messages();
    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket socket_;
    std::map<boost::asio::ip::udp::endpoint, std::string> clients_;
    bool running_;
    std::thread receive_thread_;
};