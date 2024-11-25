#pragma once
#include <boost/asio.hpp>
#include <string>
#include <thread>

class ChatClient {
public:
    ChatClient(const std::string& server_ip, uint16_t port);
    void start();

private:
    void receive_messages();
    void send_messages();
    
    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint server_endpoint_;
    bool running_;
    std::thread receive_thread_;
};