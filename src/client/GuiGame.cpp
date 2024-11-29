#include "Core.hpp"

void Core::handleCommands(std::string command)
{
    if (command.rfind(GameAction::MOVE, 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id, x, y;
        iss >> code >> id >> x >> y;
        if (id >= 0 && id < other_players.size()) {
            other_players[id].setPosition(x, y);
        } else {
            std::cout << "Erreur : ID joueur invalide." << std::endl;
        }
    } else if (command.rfind(GameAction::CONNECT, 0) == 0) {
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

void Core::handleMoove()
{
    sf::Time elapsedTime = deltaClock.restart();
    float deltaSeconds = elapsedTime.asSeconds();
    sf::Vector2f movement(0.f, 0.f);
    if (keysPressed[sf::Keyboard::Up]) {
        movement.y -= baseSpeed * deltaSeconds;
    }
    if (keysPressed[sf::Keyboard::Down]) {
        movement.y += baseSpeed * deltaSeconds;
    }
    if (keysPressed[sf::Keyboard::Left]) {
        movement.x -= baseSpeed * deltaSeconds;
    }
    if (keysPressed[sf::Keyboard::Right]) {
        movement.x += baseSpeed * deltaSeconds;
    }
    vaisseau.move(movement);

    if (movement != sf::Vector2f(0.f, 0.f)) {
        sf::Vector2f pos = vaisseau.getPosition();
        std::ostringstream messageStream;
        messageStream << GameAction::MOVE << " " << network->getId() << " " << std::round(pos.x) << " " << std::round(pos.y);
        network->send(messageStream.str());
    }
}

void Core::gui_game() {
    sprites_game["blueGalaxy"].setScale({2, 2});

    vaisseau = utils.cat("../ressources/sprites/vaisseau" + std::to_string(network->getId()) + ".png");
    vaisseau.setPosition(200, 500);
    sf::IntRect rect(0, 0, vaisseau.getGlobalBounds().width / 5, vaisseau.getGlobalBounds().height);
    vaisseau.setTextureRect(rect);
    vaisseau.setScale(3, 3);

    baseSpeed = 600.f;

    window.setFramerateLimit(fps);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                keysPressed[event.key.code] = true;
            }
            if (event.type == sf::Event::KeyReleased) {
                keysPressed[event.key.code] = false;
            }
        }

        handleMoove();

        network->print_message_queue();
        buffer = network->receive().value_or("");

        handleCommands(buffer);
        // Configure les propriétés des textes
        fpsText.setFont(font);
        fpsText.setCharacterSize(20);
        fpsText.setFillColor(sf::Color::Green);
        fpsText.setPosition(10, 10);
        latencyText.setFont(font);
        latencyText.setCharacterSize(20);
        latencyText.setFillColor(sf::Color::Green);
        latencyText.setPosition(10, 40);

        float fps = 1.f / fpsClock.restart().asSeconds();

        if (latencyClock.getElapsedTime().asSeconds() > 1.f) {
            int latency = 32;
            latencyText.setString("Ping: " + std::to_string(latency) + " ms");
            fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
            latencyClock.restart();
        }

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
        window.draw(fpsText);
        window.draw(latencyText);
        window.display();
    }
}
