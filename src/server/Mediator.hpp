#pragma once
#include "../Systems.hpp"
#include "../Registry.hpp"
#include "ISender.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>

/**
 * @brief Médiateur pour la communication entre le réseau et la logique de jeu
 * @class Mediator
 * 
 * @details Cette classe implémente le pattern Mediator pour faciliter la communication
 * entre les composants réseau et la logique de jeu. Elle permet de découpler ces
 * composants tout en assurant leur synchronisation.
 * 
 * @startuml{Mediator_pattern.png}
 * class Mediator {
 * + network: ISender*
 * + game: ISender*
 * + notify(sender: Sender, action: string, params: vector<string>, context: MediatorContext)
 * + register_network(network: ISender*)
 * + register_game(game: ISender*)
 * }
 * 
 * enum Sender {
 * NETWORK
 * GAME
 * }
 * 
 * interface ISender {
 * }
 * 
 * ServerNetwork --|> ISender
 * ServerGame --|> ISender
 * Mediator o-- ISender
 * Mediator o-- Sender
 * @enduml
 * 
 * @see ISender
 * @see ServerNetwork
 * @see ServerGame
 */

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
