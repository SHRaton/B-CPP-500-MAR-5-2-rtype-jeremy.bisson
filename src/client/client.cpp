#include "client.hpp"
#include <iostream>

ChatClient::ChatClient(const std::string& server_ip, uint16_t port)
    : socket_(io_context_),
      running_(false) {
    socket_.open(boost::asio::ip::udp::v4());
    server_endpoint_ = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address::from_string(server_ip), port);
}

ChatClient::~ChatClient() {
    stop();
}

void ChatClient::stop() {
    if (running_) {
        running_ = false;
        socket_.close();
        exit(0);
        if (receive_thread_.joinable()) {
            receive_thread_.join();
        }
    }
}

void ChatClient::start() {
    running_ = true;
    receive_thread_ = std::thread(&ChatClient::receive_messages, this);
    send_messages();
}

void ChatClient::receive_messages() {
    while (running_) {
        try {
            std::array<char, MAX_BUFFER_SIZE> buffer;
            boost::asio::ip::udp::endpoint sender_endpoint;
            
            boost::system::error_code ec;
            size_t len = socket_.receive_from(
                boost::asio::buffer(buffer), 
                sender_endpoint,
                0,
                ec
            );
            
            if (!running_) break;
            
            if (!ec && len > 0) {
                std::cout << "\rReceived: " << std::string(buffer.data(), len) 
                         << "\nMessage: " << std::flush;
            }
            else if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "Receive error: " << ec.message() << std::endl;
            }
        }
        catch (const std::exception& e) {
            if (running_) {
                std::cerr << "Receive error: " << e.what() << std::endl;
            }
        }
    }
}

void ChatClient::send_messages() {
    std::string message;
    while (running_) {
        std::cout << "Message: ";
        std::getline(std::cin, message);
        
        if (message == "exit") {
            stop();
            break;
        }
        
        try {
            socket_.send_to(boost::asio::buffer(message), server_endpoint_);
        }
        catch (const std::exception& e) {
            std::cerr << "Send error: " << e.what() << std::endl;
        }
    }
}
