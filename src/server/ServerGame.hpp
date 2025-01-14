#pragma once
#include <boost/asio.hpp>
#include <sol/sol.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <map>
#include <chrono>
#include <bitset>
#include <random>
#include <vector>
#include <filesystem>
#include <regex>
#include "../Registry.hpp"
#include "ServerNetwork.hpp"
#include "Mediator.hpp"

namespace fs = std::filesystem;

/**
 * @brief Gère la logique de jeu côté serveur
 * @class ServerGame
 * 
 * @details Cette classe gère toute la logique de jeu côté serveur, incluant
 * la gestion des timers, des collisions, du spawn des monstres et des power-ups.
 * Elle utilise Lua pour la configuration et la personnalisation du gameplay.
 * 
 * @startuml{ServerGame_class.png}
 * class ServerGame {
 * + reg: Registry
 * - io_context_: boost::asio::io_context
 * - timers: vector<unique_ptr<steady_timer>>
 * - state: GameState
 * - lua: sol::state
 * + initTimers(isAi: bool)
 * + spawnMob(mob_type: int)
 * + spawnPowerUp(powerup_type: int)
 * + checkAllCollisions()
 * }
 * 
 * enum GameState {
 * LOBBY
 * INGAME
 * }
 * 
 * ServerGame --|> ISender
 * ServerGame *-- GameState
 * ServerGame o-- Registry
 * @enduml
 * 
 * @see ISender
 * @see Registry
 * @see Mediator
 */

enum class GameState{
    LOBBY,
    INGAME
};

class JsonEntity {
    public:
        std::string type;
        std::string subtype;
        float x;
        float y;
        std::string behavior;
};

class ServerGame : public ISender {
    public:
        ServerGame(Mediator &med);

        void initTimers(bool isAi);
        void StopAllTimers();
        void positionConciliation();
        void setup_conciliation_timer(boost::asio::steady_timer& conciliation_timer_);
        void setup_position_timer(boost::asio::steady_timer& position_timer);
        void setup_collision_timer(boost::asio::steady_timer& collision_timer);
        void setup_powerup_expiration_timer(boost::asio::steady_timer& invincible_timer);
        void setup_bits_shot_timer(boost::asio::steady_timer& bits_shot_timer);
        void setup_iaMobs(boost::asio::steady_timer& ia_timer);
        void setup_laser_shot_expiration_timer(boost::asio::steady_timer& laser_shot_timer);
        void setup_super_shot_timer(boost::asio::steady_timer& super_shot_timer);
        void setup_laser_shot_timer(boost::asio::steady_timer& laser_shot_timer);
        void checkLaserExpiration();
        void setup_ia_player(boost::asio::steady_timer& player_ia_timer);
        void setup_game_over_timer(boost::asio::steady_timer& game_over_timer);
        void setup_force_shot_timer(boost::asio::steady_timer& force_shot_timer);
        void spawnMob(JsonEntity entity);
        void spawnPowerUp(JsonEntity entity);
        void spawnDecor(JsonEntity entity);
        void spawnBoss(JsonEntity entity);
        void checkAllCollisions();
        bool isColliding(const component::position& pos1, const component::position& pos2, const component::size& size1, const component::size& size2);
        bool areAllPlayersDead();
        void showAllEnityAlive();
        void loadLuaScript(const std::string& scriptPath);
        void loadJson(const std::string& jsonPath);

        void addEntityToLevel(int entityType, int x, int y, std::string filename);
        void saveHighScore();


        //Chaque fonction doit se terminer par un appel à la classe Mediator
        void handleConnect(const MediatorContext& context, const std::vector<std::string>& params);
        void handleDisconnect(const MediatorContext& context, const std::vector<std::string>& params);
        void handleMoves(const std::string& action, const MediatorContext& context, const std::vector<std::string>& params);
        void handleShoot(const MediatorContext& context, const std::vector<std::string>& params);
        void handleMobSpawn(const MediatorContext& context, const std::vector<std::string>& params){};
        void handleConciliation(const MediatorContext& context, const std::vector<std::string>& params){};
        void handlePowerUpSpawn(const MediatorContext& context, const std::vector<std::string>& params){};
        void handleColision(const MediatorContext& context, const std::vector<std::string>& params);
        void handleDeath(const MediatorContext& context, const std::vector<std::string>& params);
        void handleMobShoot(const MediatorContext& context, const std::vector<std::string>& params){};
        void handleWin(const MediatorContext& context, const std::vector<std::string>& params){};
        void handleLoose(const MediatorContext& context, const std::vector<std::string>& params){};
        void handleStart(const MediatorContext& context, const std::vector<std::string>& params);
        void handleLaserShoot(const MediatorContext& context, const std::vector<std::string>& params);
        void handleSuperShoot(const MediatorContext& context, const std::vector<std::string>& params);
        void handleScoreUpdate(const MediatorContext& context, const std::vector<std::string>& params){};
        void handleLevelEditor(const MediatorContext& context, const std::vector<std::string>& params);
        void handleHighScore(const MediatorContext& context, const std::vector<std::string>& params);


        registry reg;
        std::thread io_thread_;
        boost::asio::io_context io_context_;
        std::unique_ptr<boost::asio::steady_timer> position_timer_;
        std::unique_ptr<boost::asio::steady_timer> conciliation_timer_;
        std::unique_ptr<boost::asio::steady_timer> collision_timer_;
        std::unique_ptr<boost::asio::steady_timer> invincible_timer_;
        std::unique_ptr<boost::asio::steady_timer> ia_timer_;
        std::unique_ptr<boost::asio::steady_timer> player_ia_timer_;
        std::unique_ptr<boost::asio::steady_timer> triple_shot_expiration_timer_;
        std::unique_ptr<boost::asio::steady_timer> laser_shot_expiration_timer_;
        std::unique_ptr<boost::asio::steady_timer> super_shot_timer_;
        std::unique_ptr<boost::asio::steady_timer> laser_shot_timer_;
        std::unique_ptr<boost::asio::steady_timer> force_expiration_timer_;
        std::unique_ptr<boost::asio::steady_timer> powerup_expiration_timer_;
        std::unique_ptr<boost::asio::steady_timer> force_shot_timer_;
        std::unique_ptr<boost::asio::steady_timer> bits_shot_timer_;
        std::unique_ptr<boost::asio::steady_timer> win_timer_;
        std::unique_ptr<boost::asio::steady_timer> game_over_timer_;
        std::vector<JsonEntity> allEntities;
        Mediator &med;
        GameState state;
        sol::state lua; // Machine virtuelle Lua avec sol2
        int score;
        int highscore;
};