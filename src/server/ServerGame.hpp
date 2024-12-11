#pragma once
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <map>
#include <chrono>
#include <bitset>
#include <random>
#include <vector>
#include "../Registry.hpp"
#include "ServerNetwork.hpp"
#include "Mediator.hpp"

class ServerGame : public ISender {
    public:
        ServerGame(Mediator &med);

        void initTimers();
        void positionConciliation();
        void setup_conciliation_timer(boost::asio::steady_timer& conciliation_timer_);
        void setup_position_timer(boost::asio::steady_timer& position_timer);
        void setup_spawn_timer(boost::asio::steady_timer& spawn_timer);
        void setup_powerup_timer(boost::asio::steady_timer& powerup_timer);
        void setup_collision_timer(boost::asio::steady_timer& collision_timer);
        void setup_invincible_timer(boost::asio::steady_timer& invincible_timer);
        void setup_iaMobs(boost::asio::steady_timer& ia_timer);
        void setup_triple_shot_expiration_timer(boost::asio::steady_timer& triple_shot_timer);
        void spawnMob(int mob_type);
        void spawnPowerUp(int powerup_type);
        void checkAllCollisions();
        bool isColliding(const component::position& pos1, const component::position& pos2, const component::size& size1, const component::size& size2);
        void checkTripleShotExpiration();



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



        registry reg;
        std::thread io_thread_;
        boost::asio::io_context io_context_;
        std::unique_ptr<boost::asio::steady_timer> spawn_timer_;
        std::unique_ptr<boost::asio::steady_timer> position_timer_;
        std::unique_ptr<boost::asio::steady_timer> conciliation_timer_;
        std::unique_ptr<boost::asio::steady_timer> powerup_timer_;
        std::unique_ptr<boost::asio::steady_timer> collision_timer_;
        std::unique_ptr<boost::asio::steady_timer> invincible_timer_;
        std::unique_ptr<boost::asio::steady_timer> ia_timer_;
        std::unique_ptr<boost::asio::steady_timer> triple_shot_expiration_timer_;
        Mediator &med;
};