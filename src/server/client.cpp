#include "client.hpp"
#include <iostream>

ChatClient::ChatClient(const std::string& server_ip, uint16_t port)
    : socket_(io_context_),
      running_(false) {
    socket_.open(boost::asio::ip::udp::v4());
    server_endpoint_ = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address::from_string(server_ip), port);
}

void ChatClient::start() {
    running_ = true;
    receive_thread_ = std::thread(&ChatClient::receive_messages, this);
    send_messages();
}

void ChatClient::receive_messages() {
    while (running_) {
        std::array<char, 1024> buffer;
        boost::asio::ip::udp::endpoint sender_endpoint;
        
        size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender_endpoint);
        std::cout << "\rReceived: " << std::string(buffer.data(), len) 
                 << "\nMessage: " << std::flush;
    }
}

void ChatClient::send_messages() {
    std::string message;
    while (running_) {
        std::cout << "Message: ";
        std::getline(std::cin, message);
        
        if (message == "exit") {
            running_ = false;
            break;
        }
        
        socket_.send_to(boost::asio::buffer(message), server_endpoint_);
    }
}
