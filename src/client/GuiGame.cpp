#include "Core.hpp"
#include "../Systems.hpp"
#include "../server/ServerNetwork.hpp"

std::string Core::encode_action(GameAction action)
{
    std::bitset<5> bits(static_cast<unsigned long>(action));
    return bits.to_string();
}

void Core::handleCommands(std::string command)
{
    if (command.rfind(encode_action(GameAction::MOVE), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id, x, y;
        iss >> code >> id >> x >> y;

        // Get position components from registry
        auto &positions = reg.get_components<component::position>();
        if (id >= 0 && id < positions.size() && positions[id]) {
            // Update position component
            positions[id].value().x = x;
            positions[id].value().y = y;
        } else {
            std::cout << "Erreur : ID joueur invalide." << std::endl;
        }
    }
    else if (command.rfind(encode_action(GameAction::CONNECT), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        // Spawn new entity in the registry
        auto newPlayer = reg.spawn_entity();

        // Load sprite
        sf::Sprite vaisseau = utils.cat("../ressources/sprites/vaisseau" + std::to_string(id) + ".png");

        // Set up sprite
        vaisseau.setPosition(200, 500);
        sf::IntRect rect(0, 0, vaisseau.getGlobalBounds().width / 5, vaisseau.getGlobalBounds().height);
        vaisseau.setTextureRect(rect);
        vaisseau.setScale(3, 3);

        // Add components to the new entity
        reg.emplace_component<component::position>(newPlayer, component::position{200, 500});
        reg.emplace_component<component::velocity>(newPlayer, component::velocity{0, 0});
        reg.emplace_component<component::drawable>(newPlayer, component::drawable{vaisseau});
        reg.emplace_component<component::controllable>(newPlayer, component::controllable{false});

    } else if (command.rfind(encode_action(GameAction::UP), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        auto &positions = reg.get_components<component::position>();
        auto &drawables = reg.get_components<component::drawable>();
        auto &velocities = reg.get_components<component::velocity>();  // Ajout des vélocités

        if (id >= 0 && id < positions.size() && positions[id] && drawables[id] && velocities[id]) {
            velocities[id].value().vy = -1;  // Définit une vélocité constante vers le haut
        }
    } else if (command.rfind(encode_action(GameAction::DOWN), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        auto &positions = reg.get_components<component::position>();
        auto &drawables = reg.get_components<component::drawable>();
        auto &velocities = reg.get_components<component::velocity>();

        if (id >= 0 && id < positions.size() && positions[id] && drawables[id] && velocities[id]) {
            velocities[id].value().vy = 1;  // Définit une vélocité constante vers le bas
        }
    } else if (command.rfind(encode_action(GameAction::LEFT), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        auto &positions = reg.get_components<component::position>();
        auto &drawables = reg.get_components<component::drawable>();
        auto &velocities = reg.get_components<component::velocity>();

        if (id >= 0 && id < positions.size() && positions[id] && drawables[id] && velocities[id]) {
            velocities[id].value().vx = -1;  // Définit une vélocité constante vers la gauche
        }
    } else if (command.rfind(encode_action(GameAction::RIGHT), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        auto &positions = reg.get_components<component::position>();
        auto &drawables = reg.get_components<component::drawable>();
        auto &velocities = reg.get_components<component::velocity>();

        if (id >= 0 && id < positions.size() && positions[id] && drawables[id] && velocities[id]) {
            velocities[id].value().vx = 1;  // Définit une vélocité constante vers la droite
        }
    } else if (command.rfind(encode_action(GameAction::STOP_X), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        auto &velocities = reg.get_components<component::velocity>();

        if (id >= 0 && id < velocities.size() && velocities[id]) {
            velocities[id].value().vx = 0;  // Arrête le mouvement horizontal
        }
    } else if (command.rfind(encode_action(GameAction::STOP_Y), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        auto &velocities = reg.get_components<component::velocity>();

        if (id >= 0 && id < velocities.size() && velocities[id]) {
            velocities[id].value().vy = 0;  // Arrête le mouvement vertical
        }
    } else if (!command.empty()) {
        std::cout << "Commande inconnue : " << command << std::endl;
    }
}

void Core::load_spaceship()
{
    usleep(100);
    std::vector<std::string> messages = network->receive_all();
    std::cout << "Nombre de messages : " << messages.size() << std::endl;
    for (const auto& buffer : messages) {
        std::cout << "Message : " << buffer << std::endl;
    }

    // Handle multiple player connections
    int i = 0;
    for (const auto& buffer : messages) {
        if (i + 1 == messages.size()) {
            break;
        }
        std::cout << "Mess: " << buffer << "\n";
        if (buffer.rfind(encode_action(GameAction::CONNECT), 0) == 0) {
            std::istringstream iss(buffer);
            std::string code;
            int id;
            iss >> code >> id;

            auto newPlayer = reg.spawn_entity();
            std::cout << "Créer AUTRE sprite " << id << std::endl;

            sf::Sprite vaisseau = utils.cat("../ressources/sprites/vaisseau" + std::to_string(id) + ".png");
            vaisseau.setPosition(200, 500);
            sf::IntRect rect(0, 0, vaisseau.getGlobalBounds().width / 5, vaisseau.getGlobalBounds().height);
            vaisseau.setTextureRect(rect);
            vaisseau.setScale(3, 3);

            // Add components to the new entity
            reg.emplace_component<component::position>(newPlayer, component::position{200, 500});
            reg.emplace_component<component::velocity>(newPlayer, component::velocity{0, 0});
            reg.emplace_component<component::drawable>(newPlayer, component::drawable{vaisseau});
            reg.emplace_component<component::controllable>(newPlayer, component::controllable{false}); // Network players are not directly controllable
        }
        i++;
    }

    if (!messages.empty()) {
        // Création du joueur
        player = reg.spawn_entity();
        std::cout << "Créer NOTRE sprite " << player << std::endl;
        network->setId(player);
        std::cout << "ID DU PLAYER : " << player << "\n";
        reg.emplace_component<component::position>(player, component::position{200, 500});
        reg.emplace_component<component::velocity>(player, component::velocity{0, 0});
        sf::Sprite sprite = utils.cat("../ressources/sprites/vaisseau" + std::to_string(player) + ".png");
        sf::IntRect rect(0, 0, sprite.getGlobalBounds().width / 5, sprite.getGlobalBounds().height);
        sprite.setTextureRect(rect);
        sprite.setScale(3, 3);
        reg.emplace_component<component::drawable>(player, component::drawable{sprite});
        reg.emplace_component<component::controllable>(player, component::controllable{true});
    }
}

void Core::control_system()
{
    // Time-based movement calculation
    sf::Time elapsedTime = deltaClock.restart();
    float deltaSeconds = elapsedTime.asSeconds();
    float baseSpeed = 600.f;

    // Static variables for animation tracking
    static int currentFrame = 0;
    static float animationTimer = 0.0f;
    const float FRAME_DURATION = 0.05f;

    // Nouveaux static pour suivre l'état des touches
    static bool upSent = false;
    static bool downSent = false;
    static bool leftSent = false;
    static bool rightSent = false;

    auto const &controllables = reg.get_components<component::controllable>();
    auto &velocities = reg.get_components<component::velocity>();
    auto &positions = reg.get_components<component::position>();
    auto &drawables = reg.get_components<component::drawable>();

    for (size_t i = 0; i < controllables.size(); ++i)
    {
        auto const &controllable = controllables[i];
        auto &vel = velocities[i];
        auto &pos = positions[i];
        auto &drawable = drawables[i];

        if (controllable && vel && drawable)
        {
            if (controllable.value().is_controllable)
            {
                sf::Vector2f movement(0.f, 0.f);

                // Up movement with animation
                if (keysPressed[sf::Keyboard::Up]) {
                    movement.y -= baseSpeed * deltaSeconds;
                    animationTimer += deltaSeconds;

                    // Avance l'animation
                    if (animationTimer >= FRAME_DURATION && currentFrame < 4) {
                        currentFrame++;
                        animationTimer = 0;
                        
                        // Update texture rect for animation
                        sf::Sprite &sprite = drawable.value().sprite;
                        sprite.setTextureRect(sf::IntRect(
                            currentFrame * sprite.getTextureRect().width, 0,
                            sprite.getTextureRect().width,
                            sprite.getTextureRect().height
                        ));
                    }
                    
                    // Set velocity
                    vel.value().vy = -1;

                    // Envoie le message UP uniquement s'il n'a pas déjà été envoyé
                    if (!upSent) {
                        std::ostringstream messageStream;
                        messageStream << encode_action(GameAction::UP) << " " << network->getId();
                        network->send(messageStream.str());
                        upSent = true;
                    }
                } else {
                    // Réinitialise le flag quand la touche est relâchée
                    upSent = false;
                }

                // Animation reset when not moving up
                if (currentFrame > 0) {
                    animationTimer += deltaSeconds;
                    if (animationTimer >= FRAME_DURATION) {
                        currentFrame--;
                        animationTimer = 0;
                        // Update texture rect for animation
                        sf::Sprite &sprite = drawable.value().sprite;
                        sprite.setTextureRect(sf::IntRect(
                            currentFrame * sprite.getTextureRect().width, 0,
                            sprite.getTextureRect().width,
                            sprite.getTextureRect().height
                        ));
                    }
                }

                // Other directional movements - même logique pour DOWN, LEFT, RIGHT
                if (keysPressed[sf::Keyboard::Down]) {
                    movement.y += baseSpeed * deltaSeconds;
                    vel.value().vy = 1;
                    if (!downSent) {
                        std::ostringstream messageStream;
                        messageStream << encode_action(GameAction::DOWN) << " " << network->getId();
                        network->send(messageStream.str());
                        downSent = true;
                    }
                } else {
                    downSent = false;
                }

                if (keysPressed[sf::Keyboard::Left]) {
                    movement.x -= baseSpeed * deltaSeconds;
                    vel.value().vx = -1;
                    if (!leftSent) {
                        std::ostringstream messageStream;
                        messageStream << encode_action(GameAction::LEFT) << " " << network->getId();
                        network->send(messageStream.str());
                        leftSent = true;
                    }
                } else {
                    leftSent = false;
                }

                if (keysPressed[sf::Keyboard::Right]) {
                    movement.x += baseSpeed * deltaSeconds;
                    vel.value().vx = 1;
                    if (!rightSent) {
                        std::ostringstream messageStream;
                        messageStream << encode_action(GameAction::RIGHT) << " " << network->getId();
                        network->send(messageStream.str());
                        rightSent = true;
                    }
                } else {
                    rightSent = false;

                }

                // Reset velocity if no movement keys are pressed
                if (!keysPressed[sf::Keyboard::Left] && 
                    !keysPressed[sf::Keyboard::Right]) {
                    if (vel.value().vx != 0) {
                        // Send STOP_X message only when velocity changes from non-zero to zero
                        std::ostringstream messageStream;
                        messageStream << encode_action(GameAction::STOP_X) << " " << network->getId();
                        network->send(messageStream.str());
                    }
                    vel.value().vx = 0;
                }
                if (!keysPressed[sf::Keyboard::Up] && 
                    !keysPressed[sf::Keyboard::Down]) {
                    if (vel.value().vy != 0) {
                        // Send STOP_Y message only when velocity changes from non-zero to zero
                        std::ostringstream messageStream;
                        messageStream << encode_action(GameAction::STOP_Y) << " " << network->getId();
                        network->send(messageStream.str());
                    }
                    vel.value().vy = 0;
                }
            }
        }
    }
}

void Core::gui_game() {
    load_spaceship();
    sf::Event event;
    while (window.isOpen()) {;
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
        network->print_message_queue();
        buffer = network->receive().value_or("");
        handleCommands(buffer);
        control_system();
        sys.position_system(reg);
        window.clear();
        sys.draw_system(reg, window);
        window.display();
    }
}
