#pragma once
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <map>
#include <chrono>
#include <bitset>
#include <vector>
#include <iostream>
#include <fstream>
#include "Mediator.hpp"
#include "Command.hpp"

/**
 * @brief Gère la communication réseau du serveur
 * @class ServerNetwork
 * 
 * @details Cette classe implémente l'interface ISender et gère toutes les communications
 * réseau du serveur, incluant la connexion des clients, l'envoi et la réception des messages.
 * 
 * @startuml{ServerNetwork_class.png}
 * class ServerNetwork {
 * - io_context_: boost::asio::io_context
 * - socket_: boost::asio::ip::udp::socket
 * - clients_: map<endpoint, ClientInfo>
 * - running_: bool
 * - receive_thread_: thread
 * + ServerNetwork(port: uint16_t, med: Mediator&)
 * + start()
 * - receive_messages()
 * - decode_message(message: string): GameMessage
 * - broadcast_message(message: string)
 * }
 * 
 * enum GameAction {
 * NONE
 * SHOOT
 * MOVE
 * {and other actions...}
 * }
 * 
 * struct GameMessage {
 * + action: GameAction
 * + arguments: vector<string>
 * }
 * 
 * ServerNetwork --|> ISender
 * ServerNetwork *-- GameMessage
 * GameMessage *-- GameAction
 * @enduml
 */

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
    ENTITY_SPAWN = 0b1001,  // 9
    RESPAWN     = 0b1010,  // 10
    MOB_SPAWN   = 0b1011,  // 11
    MOB_SHOOT   = 0b1100,  // 12
    LASER_SHOOT = 0b1101,  // 13
    SUPER_SHOOT = 0b1110,  // 14
    QUIT        = 0b1111,  // 15
    STOP_Y      = 0b10000, // 16
    STOP_X      = 0b10001,  // 17
    DEATH      = 0b10010, // 18
    LEVEL_EDITOR = 0b10011, // 19
    COLLISION   = 0b10100, // 20
    WIN        = 0b10101, // 21
    START     = 0b10111, // 23
    LOOSE     = 0b10110, // 22
    SCORE_UPDATE = 0b11001, // 25
    SAVE_REPLAY = 0b11010, // 26
    DISCONNECT  = 0b11011, // 27
    PLAY_REPLAY = 0b11100, // 28
    HIGHSCORE = 0b11101, // 29
    CONNECT     = 0b11111, // 31
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

    //For "replay" feature
    void saveCommands(const std::string& filename);
    std::vector<Command> loadCommands(const std::string& filename);


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
    void handleMobShoot(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleWin(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleLoose(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleStart(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleLaserShoot(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleSuperShoot(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleScoreUpdate(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleLevelEditor(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleHighScore(const MediatorContext& context, const std::vector<std::string>& params) override;
    void handleSaveReplay();
    void handlePlayReplay();

    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket socket_;
    std::map<boost::asio::ip::udp::endpoint, ClientInfo> clients_;
    bool running_;
    std::thread receive_thread_;
    std::chrono::steady_clock::time_point launch_time_;
    std::vector<Command> commands_;

    Mediator& med;
};