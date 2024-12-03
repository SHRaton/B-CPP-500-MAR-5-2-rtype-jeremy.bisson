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
        }
    } else if (sender == Sender::GAME) { //Communication entre Game et Network
        if (action == "CONNECT") {
            network->handleConnect(context, params);
        } else if (action == "DISCONNECT") {
            network->handleDisconnect(context, params);
        } else if (action == "MOVE") {
        }
    }
}
