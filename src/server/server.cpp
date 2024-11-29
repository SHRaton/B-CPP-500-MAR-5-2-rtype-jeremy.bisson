#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <map>
#include <iostream>
#include <chrono>
#include <sstream>
#include "server.hpp"

Server::Server(uint16_t port)
    : socket_(io_context_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
      running_(false) {
    std::cout << Color::GREEN << "[Console] : Démarrage du serveur sur le port " << port << Color::RESET << std::endl;
}



std::string Server::encode_action(GameAction action) {
    std::bitset<5> bits(static_cast<unsigned long>(action));
    return bits.to_string();
}

GameMessage Server::decode_message(const std::string& message) {
    GameMessage msg;
    std::istringstream iss(message);
    
    std::string binary_code;
    iss >> binary_code;
    
    try {
        std::bitset<5> bits(binary_code);
        msg.action = static_cast<GameAction>(bits.to_ulong());
    } catch (...) {
        msg.action = GameAction::NONE;
    }
    
    std::string arg;
    while (iss >> arg) {
        msg.arguments.push_back(arg);
    }
    
    return msg;
}

std::string Server::get_action_name(GameAction action) {
    switch (action) {
        case GameAction::SHOOT: return "SHOOT";
        case GameAction::MOVE: return "MOVE";
        case GameAction::DOWN: return "DOWN";
        case GameAction::LEFT: return "LEFT";
        case GameAction::RIGHT: return "RIGHT";
        case GameAction::JUMP: return "JUMP";
        case GameAction::CROUCH: return "CROUCH";
        case GameAction::POWER_UP: return "POWER_UP";
        case GameAction::SHIELD: return "SHIELD";
        case GameAction::RESPAWN: return "RESPAWN";
        case GameAction::CONNECT: return "CONNECT";
        case GameAction::DISCONNECT: return "DISCONNECT";
        case GameAction::QUIT: return "QUIT";
        default: return "UNKNOWN";
    }
}

void Server::handle_disconnect(const boost::asio::ip::udp::endpoint& client) {
    std::cout << Color::RED << "[Console] : Un joueur s'est déconnecté : " 
              << client.address().to_string() << ":" 
              << client.port() << Color::RESET << std::endl;
    clients_.erase(client);
}

Entity Server::handle_connect(const boost::asio::ip::udp::endpoint& client) {
    clients_[client] = ClientInfo{"", true};


    std::cout << Color::YELLOW << "[Console] : Un joueur s'est connecté : " 
              << client.address().to_string() 
              << ":" << client.port() << Color::RESET << std::endl;

    Entity player = reg.spawn_entity();
    reg.emplace_component<component::position>(player, component::position{0, 0});
    reg.emplace_component<component::velocity>(player, component::velocity{0, 0});
    reg.emplace_component<component::drawable>(player, component::drawable{"./ressources/sprites/jinx.png"});
    reg.emplace_component<component::controllable>(player, component::controllable{true});
    reg.emplace_component<component::health>(player, component::health{100});
    reg.emplace_component<component::damage>(player, component::damage{10});
    return player;
}

void Server::handle_game_message(const boost::asio::ip::udp::endpoint& sender, const GameMessage& msg) {
    if (msg.action == GameAction::CONNECT) {
        Entity player = handle_connect(sender);
        socket_.send_to(boost::asio::buffer("OK " + std::to_string(player)), sender);
        return;
    }
    else if (msg.action == GameAction::DISCONNECT) {
        handle_disconnect(sender);
        return;
    }

    // Pour les autres messages
    std::string action_name = get_action_name(msg.action);
    std::ostringstream log_message;
    log_message << Color::BLUE << "[Action] Player [" << sender.address().to_string()
                << ":" << sender.port() << "] " << std::endl;
    log_message << "Binary: " << encode_action(msg.action) << " (" << action_name << ")" << std::endl;
    log_message << "Arguments:";
    for (const auto& arg : msg.arguments) {
        log_message << " " << arg;
    }
    log_message << Color::RESET;
    std::cout << log_message.str() << std::endl;

    std::ostringstream original_message;
    original_message << encode_action(msg.action);
    for (const auto& arg : msg.arguments) {
        original_message << " " << arg;
    }
    broadcast_message(sender, original_message.str());


    Mediator::notify(Sender::CLIENT, action_name, msg.arguments);
}

void Server::broadcast_message(const boost::asio::ip::udp::endpoint& sender, const std::string& message) {
    if (clients_.size() > 1) {
        for (const auto& client : clients_) {
            if (client.first != sender) {
                socket_.send_to(boost::asio::buffer(message), client.first);
                std::cout << Color::GREEN << "[Server] Message forwarded to: [" 
                         << client.first.address().to_string() << ":" 
                         << client.first.port() << "]" << Color::RESET << std::endl;
            }
        }
    }
}

void Server::receive_messages() {
    while (running_) {
        std::array<char, 1024> buffer;
        boost::asio::ip::udp::endpoint sender_endpoint;
        
        size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender_endpoint);
        std::string message(buffer.data(), len);
        
        GameMessage game_msg = decode_message(message);
        handle_game_message(sender_endpoint, game_msg);
    }
}

void Server::start() {
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::drawable>();
    reg.register_component<component::controllable>();
    reg.register_component<component::health>();
    reg.register_component<component::damage>();


    running_ = true;
    receive_thread_ = std::thread(&Server::receive_messages, this);
    while(running_) {
        io_context_.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
}