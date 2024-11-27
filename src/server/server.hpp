#pragma once
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <map>
#include <chrono>
#include <bitset>
#include <vector>

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
}

enum class GameAction {
    NONE        = 0b0000,  // 0
    SHOOT       = 0b0001,  // 1
    MOVE        = 0b0010,  // 2
    DOWN        = 0b0011,  // 3
    LEFT        = 0b0100,  // 4
    RIGHT       = 0b0101,  // 5
    JUMP        = 0b0110,  // 6
    CROUCH      = 0b0111,  // 7
    POWER_UP    = 0b1000,  // 8
    SHIELD      = 0b1001,  // 9
    RESPAWN     = 0b1010,  // 10
    DISCONNECT  = 0b11011, // 27
    CONNECT     = 0b11111, // 31
    QUIT        = 0b1111   // 15
};

struct GameMessage {
    GameAction action;
    std::vector<std::string> arguments;
};

class Server {
public:
    Server(uint16_t port);
    void start();

private:
    struct ClientInfo {
        std::string name;
        bool connected;
    };

    void receive_messages();
    
    GameMessage decode_message(const std::string& message);
    std::string encode_action(GameAction action);
    std::string get_action_name(GameAction action);
    void handle_game_message(const boost::asio::ip::udp::endpoint& sender, const GameMessage& msg);
    void broadcast_message(const boost::asio::ip::udp::endpoint& sender, const std::string& message);
    void handle_disconnect(const boost::asio::ip::udp::endpoint& client);
    void handle_connect(const boost::asio::ip::udp::endpoint& client);

    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket socket_;
    std::map<boost::asio::ip::udp::endpoint, ClientInfo> clients_;
    bool running_;
    std::thread receive_thread_;
};