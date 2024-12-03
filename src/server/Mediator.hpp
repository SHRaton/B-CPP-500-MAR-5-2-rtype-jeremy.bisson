#pragma once
#include "../Systems.hpp"
#include "../Registry.hpp"
#include "ISender.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>


enum class Sender {
    NETWORK,
    GAME
};

class Mediator {
public:
    Mediator() = default;
    Mediator(ISender* network, ISender* game)
        : network(network), game(game) {}
    void notify(Sender sender, const std::string& action, const std::vector<std::string>& params,
                       const MediatorContext& context = MediatorContext());

    void register_network(ISender* network) { this->network = network;}
    void register_game(ISender* game) { this->game = game;}

    ISender *network;
    ISender *game;
};
