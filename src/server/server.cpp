#include "server.hpp"
#include <iostream>

ChatServer::ChatServer(uint16_t port)
    : socket_(io_context_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
      running_(false) {
    std::cout << "Server started on port " << port << std::endl;
}

void ChatServer::start() {
    running_ = true;
    receive_thread_ = std::thread(&ChatServer::receive_messages, this);
    while(running_) {
        io_context_.poll(); // Gère les événements en attente
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
}

void ChatServer::receive_messages() {
    while (running_) {
        std::array<char, 1024> buffer;
        boost::asio::ip::udp::endpoint sender_endpoint;
        
        size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender_endpoint);
        std::string message(buffer.data(), len);
        
        for (const auto& client : clients_) {
            if (client.first != sender_endpoint) {
                socket_.send_to(boost::asio::buffer(message), client.first);
            }
        }
        
        if (clients_.find(sender_endpoint) == clients_.end()) {
            clients_[sender_endpoint] = "";
            std::cout << "New client connected: " << sender_endpoint.address().to_string() 
                     << ":" << sender_endpoint.port() << std::endl;
        }
    }
}