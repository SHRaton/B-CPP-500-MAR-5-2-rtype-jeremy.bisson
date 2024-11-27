#include "server.hpp"
#include "Mediator.hpp"

void Mediator::notify(Sender sender, const std::string& action, const std::vector<std::string>& params) {
    if (sender == Sender::CLIENT) {
        if (action == "CONNECT") {
            std::cout << Color::GREEN << "Client: " << action << " " << Color::RESET << std::endl;
        } else if (action == "DISCONNECT") {
            std::cout << Color::RED << "Client: " << action << " " << Color::RESET << std::endl;
        } else if (action == "MOVE"){
            int playerId = std::stoi(params[0]);
            int x = std::stoi(params[1]);
            int y = std::stoi(params[2]);
            //Systems::player_control_system(reg, playerId, x, y); AJOUTER LE REG EN ARGUMENT ??
        }
    } else if (sender == Sender::SERVER) {
        if (action == "MOVE") {
            //ENVOYER A TOUT LES CLIENTS LA POSITION DU JOUEUR
        }
    }
}