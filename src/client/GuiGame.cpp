#include "Core.hpp"

void Core::handleCommands(std::string command)
{
    if (command.rfind("00100", 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id, x, y;
        iss >> code >> id >> x >> y;
        if (id >= 0 && id < other_players.size()) {
            other_players[id].setPosition(x, y);
        } else {
            std::cout << "Erreur : ID joueur invalide." << std::endl;
        }
    } else if (command.rfind("11111", 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;
        sf::Sprite vaisseau = utils.cat("../ressources/sprites/vaisseau" + std::to_string(id) + ".png");
        vaisseau.setPosition(200, 500);
        sf::IntRect rect(0, 0, vaisseau.getGlobalBounds().width / 5, vaisseau.getGlobalBounds().height);
        vaisseau.setTextureRect(rect);
        vaisseau.setScale(3, 3);
        other_players[id] = vaisseau;
        std::cout << "Vaisseau " << id << " créé avec succès !" << std::endl;
    } else if (!command.empty()) {
        std::cout << "Commande inconnue : " << command << std::endl;
    }
}

void Core::gui_game() {
    sprites_game["blueGalaxy"].setScale({2, 2});

    sf::Sprite vaisseau = utils.cat("../ressources/sprites/vaisseau" + std::to_string(network->getId()) + ".png");
    vaisseau.setPosition(200, 500);
    sf::IntRect rect(0, 0, vaisseau.getGlobalBounds().width / 5, vaisseau.getGlobalBounds().height);
    vaisseau.setTextureRect(rect);
    vaisseau.setScale(3, 3);

    int pressed = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (event.type == sf::Event::MouseMoved) {
                sprites_menu["play"].setHovered(sprites_menu["play"].isMouseOver(window));
                sprites_menu["quit"].setHovered(sprites_menu["quit"].isMouseOver(window));
            }
            if (event.type == sf::Event::KeyPressed) {
                pressed = event.key.code;
            }
            if (event.type == sf::Event::KeyReleased) {
                pressed = 0;
            }
        }


        if (pressed == sf::Keyboard::Up) {
            vaisseau.move(0, -4);
            sf::Vector2f pos = vaisseau.getPosition();
            std::ostringstream messageStream;
            messageStream << GameAction::MOVE << " " << network->getId() << " " << pos.x << " " << pos.y;
            network->send(GameAction::MOVE + messageStream.str());
        }
        if (pressed == sf::Keyboard::Down) {
            vaisseau.move(0, 4);
            sf::Vector2f pos = vaisseau.getPosition();
            std::ostringstream messageStream;
            messageStream << GameAction::MOVE << " " << network->getId() << " " << pos.x << " " << pos.y;
            network->send(GameAction::MOVE + messageStream.str());
        }
        if (pressed == sf::Keyboard::Left) {
            vaisseau.move(-4, 0);
            sf::Vector2f pos = vaisseau.getPosition();
            std::ostringstream messageStream;
            messageStream << GameAction::MOVE << " " << network->getId() << " " << pos.x << " " << pos.y;
            network->send(GameAction::MOVE + messageStream.str());
        }
        if (pressed == sf::Keyboard::Right) {
            vaisseau.move(4, 0);
            sf::Vector2f pos = vaisseau.getPosition();
            std::ostringstream messageStream;
            messageStream << GameAction::MOVE << " " << network->getId() << " " << pos.x << " " << pos.y;
            network->send(GameAction::MOVE + messageStream.str());
        }

        network->print_message_queue();
        buffer = network->receive().value_or("");

        handleCommands(buffer);

        for (auto& [name, sprite] : sprites_game) {
            if (name == "blueGalaxy") {
                sprite.update_xy(1920, 0);
            } else {
                sprite.update();
            }
        }
        window.clear();
        for (const auto& name : drawOrder_game) {
            window.draw(sprites_game[name].getSprite());
        }
        window.draw(vaisseau);
        int i = 0;
        for (const auto& name : other_players) {
            window.draw(other_players[i]);
            i++;
        }
        window.display();
    }
}
