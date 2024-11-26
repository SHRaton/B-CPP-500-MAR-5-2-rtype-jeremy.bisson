#pragma once
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <atomic>

class ChatClient {
public:
    ChatClient(const std::string& server_ip, uint16_t port);
    ~ChatClient();
    void start();
    void stop();

private:
    void receive_messages();
    void send_messages();
    
    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint server_endpoint_;
    std::atomic<bool> running_;
    std::thread receive_thread_;
    static constexpr size_t MAX_BUFFER_SIZE = 1024;
};