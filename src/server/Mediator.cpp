#include "Mediator.hpp"

void Mediator::notify(Sender sender, const std::string& action, const std::vector<std::string>& params,
                      const MediatorContext& context)
{
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
        } else if (action == "START") {
            game->handleStart(context, params);
        } else if (action == "LASER_SHOOT") {
            game->handleLaserShoot(context, params);
        } else if (action == "SUPER_SHOOT") {
            game->handleSuperShoot(context, params);
        } else if (action == "ENTITY_SPAWN") {
            game->handlePowerUpSpawn(context, params);
        } else if (action == "MOB_SPAWN") {
            game->handleMobSpawn(context, params);
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
        } else if (action == "MOB_SHOOT") {
            network->handleMobShoot(context, params);
        } else if (action == "WIN") {
            network->handleWin(context, params);
        } else if (action == "LOOSE") {
            network->handleLoose(context, params);
        } else if (action == "START") {
            network->handleStart(context, params);
        } else if (action == "LASER_SHOOT") {
            network->handleLaserShoot(context, params);
        } else if(action == "SUPER_SHOOT") {
            network->handleSuperShoot(context, params);
        } else if (action == "SCORE_UPDATE") {
            network->handleScoreUpdate(context, params);
        }
    }
}
