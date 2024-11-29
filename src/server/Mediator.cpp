#include "server.hpp"
#include "Mediator.hpp"

void Mediator::notify(Sender sender, const std::string& action, const std::vector<std::string>& params, registry& reg) {
    if (sender == Sender::CLIENT) { //Communication entre Network et Systems
        if (action == "CONNECT") {
            std::cout << Color::GREEN << "Client: " << action << " " << Color::RESET << std::endl;
        } else if (action == "DISCONNECT") {
            std::cout << Color::RED << "Client: " << action << " " << Color::RESET << std::endl;
        } else if (action == "MOVE"){
            int playerId = std::stoi(params[0]);
            int x = std::stoi(params[1]);
            int y = std::stoi(params[2]);
            Systems::player_control_system(reg, playerId, x, y);
        }
    } else if (sender == Sender::SERVER) { //Communication entre Systems et Network
        //TODO: Voir si il y a besoin de faire quelque chose ici
    }
}