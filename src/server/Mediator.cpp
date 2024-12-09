#include "Mediator.hpp"

void Mediator::notify(Sender sender, const std::string& action, const std::vector<std::string>& params,
                      const MediatorContext& context)
{
    std::cout << "Mediator::notify" << std::endl;
    if (sender == Sender::NETWORK) { //Communication entre Network et Game
        if (action == "CONNECT") {
            game->handleConnect(context, params);
        } else if (action == "DISCONNECT") {
            game->handleDisconnect(context, params);
        } else if (action == "UP") {
            game->handleMoves(action, context, params);
        } else if (action == "DOWN") {
            game->handleMoves(action, context, params);
        } else if (action == "LEFT") {
            game->handleMoves(action, context, params);
        } else if (action == "RIGHT") {
            game->handleMoves(action, context, params);
        } else if (action == "STOP_X") {
            game->handleMoves(action, context, params);
        } else if (action == "STOP_Y") {
            game->handleMoves(action, context, params);
        } else if (action == "SHOOT") {
            game->handleShoot(context, params);
        }
    } else if (sender == Sender::GAME) { //Communication entre Game et Network
        if (action == "CONNECT") {
            network->handleConnect(context, params);
        } else if (action == "DISCONNECT") {
            network->handleDisconnect(context, params);
        } else if (action == "UP") {
            network->handleMoves(action, context, params);
        } else if (action == "DOWN") {
            network->handleMoves(action, context, params);
        } else if (action == "LEFT") {
            network->handleMoves(action, context, params);
        } else if (action == "RIGHT") {
            network->handleMoves(action, context, params);
        } else if (action == "STOP_X") {
            network->handleMoves(action, context, params);
        } else if (action == "STOP_Y") {
            network->handleMoves(action, context, params);
        } else if (action == "SHOOT") {
            network->handleShoot(context, params);
        } else if (action == "MOB_SPAWN") {
            network->handleMobSpawn(context, params);
        } else if (action == "MOVE") { //Pour la conciliation client side
            network->handleConciliation(context, params);
        } else if (action == "POWERUP_SPAWN") {
            network->handlePowerUpSpawn(context, params);
        } else if (action == "COLISION") {
            network->handleColision(context, params);
        } else if (action == "DEATH") {
            network->handleDeath(context, params);
        }
    }
}
