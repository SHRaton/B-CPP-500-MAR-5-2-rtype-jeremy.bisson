#pragma once
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <map>
#include <chrono>
#include <bitset>
#include <vector>
#include "../Registry.hpp"
#include "ServerNetwork.hpp"
#include "Mediator.hpp"

class ServerGame : public ISender {
    public:
        ServerGame(Mediator &med);

        void initTimers();
        void setup_spawn_timer(boost::asio::steady_timer& spawn_timer);


        //Chaque fonction doit se terminer par un appel à la classe Mediator
        void handleConnect(const MediatorContext& context, const std::vector<std::string>& params);
        void handleDisconnect(const MediatorContext& context, const std::vector<std::string>& params);
        void spawnMob(int mob_type);



        registry reg;
        std::thread io_thread_;
        boost::asio::io_context io_context_;
        std::unique_ptr<boost::asio::steady_timer> spawn_timer_;
        Mediator &med;
};