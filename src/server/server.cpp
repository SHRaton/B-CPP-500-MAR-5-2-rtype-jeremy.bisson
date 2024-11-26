#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <map>
#include <iostream>
#include <chrono>
#include "server.hpp"

Server::Server(uint16_t port)
    : socket_(io_context_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
      running_(false) {
    std::cout << Color::GREEN << "[Console] : Démarrage du serveur sur le port " << port << Color::RESET << std::endl;
}

void Server::start() {
    running_ = true;
    receive_thread_ = std::thread(&Server::receive_messages, this);
    while(running_) {
        io_context_.poll();
        check_timeouts();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
}

void Server::check_timeouts() {
    auto now = std::chrono::steady_clock::now();
    auto it = clients_.begin();
    while (it != clients_.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second.last_seen);
        if (duration.count() > TIMEOUT_MS) {
            std::cout << Color::RED << "[Console] : Un joueur s'est déconnecté : " 
                     << it->first.address().to_string() << ":" 
                     << it->first.port() << Color::RESET << std::endl;
            it = clients_.erase(it);
        } else {
            ++it;
        }
    }
}

void Server::receive_messages() {
    while (running_) {
        std::array<char, 1024> buffer;
        boost::asio::ip::udp::endpoint sender_endpoint;
        
        size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender_endpoint);
        std::string message(buffer.data(), len);
        
        std::cout << Color::BLUE << "[Message] Received from: [" << sender_endpoint.address().to_string() 
                 << ":" << sender_endpoint.port() << "] : " << message 
                 << Color::RESET << std::endl;

        // Mise à jour ou ajout du client
        auto now = std::chrono::steady_clock::now();
        if (clients_.find(sender_endpoint) == clients_.end()) {
            clients_[sender_endpoint] = ClientInfo{"", now};
            std::cout << Color::YELLOW << "[Console] : Un joueur s'est connecté : " 
                     << sender_endpoint.address().to_string() 
                     << ":" << sender_endpoint.port() << Color::RESET << std::endl;
        } else {
            clients_[sender_endpoint].last_seen = now;
        }

        if (clients_.size() > 1) {
            for (const auto& client : clients_) {
                if (client.first != sender_endpoint) {
                    socket_.send_to(boost::asio::buffer(message), client.first);
                    
                    std::cout << Color::GREEN << "[Server] Forwarding to: [" 
                             << client.first.address().to_string() << ":" 
                             << client.first.port() << "]" 
                             << Color::RESET << std::endl;
                }
            }
        }
    }
}