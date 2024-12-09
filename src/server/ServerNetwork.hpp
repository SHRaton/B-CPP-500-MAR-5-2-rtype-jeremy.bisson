#pragma once
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <map>
#include <chrono>
#include <bitset>
#include <vector>
#include "Mediator.hpp"

namespace Colors {
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
    UP          = 0b0110,  // 6
    STOP        = 0b0111,  // 7
    POWER_UP    = 0b1000,  // 8
    POWER_UP_SPAWN = 0b1001,  // 9
    RESPAWN     = 0b1010,  // 10
    MOB_SPAWN   = 0b1011,  // 11
    QUIT        = 0b1111,  // 15
    STOP_Y      = 0b10000, // 16
    STOP_X      = 0b10001,  // 17
    DISCONNECT  = 0b11011, // 27
    CONNECT     = 0b11111, // 31
    DEATH      = 0b10010, // 18
    COLLISION   = 0b10100 // 20
};

struct GameMessage {
    GameAction action;
    std::vector<std::string> arguments;
};

class ServerNetwork : public ISender {
public:
    ServerNetwork(uint16_t port, Mediator& med);
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
    void broadcast_message(const std::string& message);


    //ISender implementation
    void handleDisconnect(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleConnect(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleMoves(const std::string& action, const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleShoot(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleMobSpawn(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleConciliation(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handlePowerUpSpawn(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleColision(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleDeath(const MediatorContext& context, const std::vector<std::string>& params) override;

    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket socket_;
    std::map<boost::asio::ip::udp::endpoint, ClientInfo> clients_;
    bool running_;
    std::thread receive_thread_;

    Mediator& med;
};