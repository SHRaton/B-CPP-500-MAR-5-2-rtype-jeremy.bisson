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

        auto &positions = reg.get_components<component::position>();
        if (id >= 0 && id < positions.size() && positions[id]) {
            positions[id].value().x = x;
            positions[id].value().y = y;
        } else {
            std::cout << "Erreur : ID de l'Entitée invalide." << std::endl;
        }
    } else if (command.rfind(encode_action(GameAction::MOB_SPAWN), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int mob_type, x, y;
        iss >> code >> mob_type >> x >> y;
        auto newMob = reg.spawn_entity();
        sf::Sprite mob = utils.cat("../ressources/sprites/mob" + std::to_string(mob_type) + ".png");
        mob.setPosition(x, y);
        mob.setScale(3, 3);
        reg.emplace_component<component::position>(newMob, component::position{x, y});
        if (mob_type == 0) {
            reg.emplace_component<component::health>(newMob, component::health{300});
            reg.emplace_component<component::damage>(newMob, component::damage{10});
            reg.emplace_component<component::velocity>(newMob, component::velocity{-1, 0});
        } else if (mob_type == 1) {
            reg.emplace_component<component::health>(newMob, component::health{100});
            reg.emplace_component<component::damage>(newMob, component::damage{40});
            reg.emplace_component<component::velocity>(newMob, component::velocity{-2, 0});
        }
        reg.emplace_component<component::drawable>(newMob, component::drawable{mob});
        std::cout << "MOB" << newMob << "SPAWNED AT " << x << " / " << y << std::endl;

    } else if (command.rfind(encode_action(GameAction::CONNECT), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        // Spawn new entity in the registry
        auto newPlayer = reg.spawn_entity();

        // Load sprite based on player number, NOT nb_player counter
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
        auto &velocities = reg.get_components<component::velocity>();

        if (id >= 0 && id < positions.size() && positions[id] && drawables[id] && velocities[id]) {
            velocities[id].value().vy = -baseSpeed;
            
            // Animer le sprite vers le haut
            sf::Sprite& sprite = drawables[id].value().sprite;
            if (animState.currentFrame < MAX_UP_FRAME) {
                sprite.setTextureRect(sf::IntRect(
                    MAX_UP_FRAME * sprite.getTextureRect().width,
                    0,
                    sprite.getTextureRect().width,
                    sprite.getTextureRect().height
                ));
            }
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
            velocities[id].value().vy = baseSpeed;
            
            // Animer le sprite vers le bas
            sf::Sprite& sprite = drawables[id].value().sprite;
            if (animState.currentFrame > MIN_DOWN_FRAME) {
                sprite.setTextureRect(sf::IntRect(
                    MIN_DOWN_FRAME * sprite.getTextureRect().width,
                    0,
                    sprite.getTextureRect().width,
                    sprite.getTextureRect().height
                ));
            }
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
            velocities[id].value().vx = -baseSpeed;
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
            velocities[id].value().vx = baseSpeed;
        }
    } else if (command.rfind(encode_action(GameAction::STOP_X), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        auto &velocities = reg.get_components<component::velocity>();

        if (id >= 0 && id < velocities.size() && velocities[id]) {
            velocities[id].value().vx = 0;
        }
    } else if (command.rfind(encode_action(GameAction::STOP_Y), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        auto &velocities = reg.get_components<component::velocity>();
        auto &drawables = reg.get_components<component::drawable>();

        if (id >= 0 && id < velocities.size() && velocities[id] && drawables[id]) {
            velocities[id].value().vy = 0;
            
            // Retour à l'animation neutre
            sf::Sprite& sprite = drawables[id].value().sprite;
            sprite.setTextureRect(sf::IntRect(
                NEUTRAL_FRAME * sprite.getTextureRect().width,
                0,
                sprite.getTextureRect().width,
                sprite.getTextureRect().height
            ));
        }
    } else if (command.rfind(encode_action(GameAction::SHOOT), 0) == 0) {
        std::istringstream iss(command);
        std::string code;
        int id;
        iss >> code >> id;

        auto &positions = reg.get_components<component::position>();
        
        if (id >= 0 && id < positions.size() && positions[id]) {
            Entity missile = reg.spawn_entity();
            reg.emplace_component<component::position>(missile, component::position{positions[id].value().x + 100, positions[id].value().y});
            reg.emplace_component<component::velocity>(missile, component::velocity{1, 0});
            sf::Sprite sprite = utils.cat("../ressources/sprites/shoot.png");
            sf::IntRect rect(0, 0, sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height);
            reg.emplace_component<component::drawable>(missile, component::drawable{sprite});
            reg.emplace_component<component::controllable>(missile, component::controllable{false});
        }
    } else if (!command.empty()) {
        std::cout << "Commande inconnue : " << command << std::endl;
    }
}

void Core::load_spaceship()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::vector<std::string> messages = network->receive_all();
    if (messages.empty()) {
        throw std::runtime_error("Erreur de connexion au serveur!");
    }

    // Handle multiple player connections
    int i = 0;
    for (const auto& buffer : messages) {
        if (messages[i].rfind("OK", 0) == 0) {
            break;
        }
        std::cout << "Mess: " << buffer << "\n";
        if (buffer.rfind(encode_action(GameAction::CONNECT), 0) == 0) {
            nb_player++;
            std::istringstream iss(buffer);
            std::string code;
            int id;
            iss >> code >> id;

            auto newPlayer = reg.spawn_entity();
            std::cout << "Créer AUTRE sprite " << i << std::endl;

            sf::Sprite vaisseau = utils.cat("../ressources/sprites/vaisseau" + std::to_string(i) + ".png");
            vaisseau.setPosition(200, 500);
            sf::IntRect rect(vaisseau.getGlobalBounds().width / 5 * 2, 0, 
                     vaisseau.getGlobalBounds().width / 5, 
                     vaisseau.getGlobalBounds().height);
            vaisseau.setTextureRect(rect);
            vaisseau.setScale(3, 3);

            // Add components to the new entity
            reg.emplace_component<component::position>(newPlayer, component::position{200, 500});
            reg.emplace_component<component::velocity>(newPlayer, component::velocity{0, 0});
            reg.emplace_component<component::drawable>(newPlayer, component::drawable{vaisseau});
            reg.emplace_component<component::controllable>(newPlayer, component::controllable{false});
        }
        i++;
    }

    if (!messages.empty()) {
        // Création du joueur
        player = reg.spawn_entity();
        network->setId(player);
        std::cout << "Créer NOTRE sprite " << player << std::endl;
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

//===================================ANIMATION=================================


void Core::handle_vertical_movement(float deltaSeconds, std::optional<component::velocity>& vel,
                                    std::optional<component::drawable>& drawable, std::optional<component::position>& pos)
{
    if (!vel || !drawable) return;

    // Handle upward movement
    if (keysPressed[sf::Keyboard::Up]) {
        if (pos.value().y < 0) {
            handle_vertical_stop(vel);
        } else {
            vel->vy = -baseSpeed;
            animate_up(deltaSeconds, drawable);
            send_input_if_needed(GameAction::UP, inputState.upSent);
        }
    } 
    // Handle downward movement
    else if (keysPressed[sf::Keyboard::Down]) {
        if (pos.value().y > 970) {
            handle_vertical_stop(vel);
        } else {
            vel->vy = baseSpeed;
            animate_down(deltaSeconds, drawable);
            send_input_if_needed(GameAction::DOWN, inputState.downSent);
        }
    }
    // Return to neutral position when no vertical movement
    else {
        inputState.upSent = false;
        inputState.downSent = false;
        animate_to_neutral(deltaSeconds, drawable);
        handle_vertical_stop(vel);
    }
}

void Core::animate_up(float deltaSeconds, std::optional<component::drawable>& drawable)
{
    if (!drawable) return;
    
    animState.animationTimer += deltaSeconds;
    if (animState.animationTimer >= FRAME_DURATION) {
        animState.animationTimer = 0;
        if (animState.currentFrame < MAX_UP_FRAME) {
            animState.currentFrame++;
            update_sprite_frame(drawable->sprite);
        }
    }
}

void Core::animate_down(float deltaSeconds, std::optional<component::drawable>& drawable)
{
    if (!drawable) return;
    
    animState.animationTimer += deltaSeconds;
    if (animState.animationTimer >= FRAME_DURATION) {
        animState.animationTimer = 0;
        if (animState.currentFrame > MIN_DOWN_FRAME) {
            animState.currentFrame--;
            update_sprite_frame(drawable->sprite);
        }
    }
}

void Core::animate_to_neutral(float deltaSeconds, std::optional<component::drawable>& drawable)
{
    if (!drawable) return;
    
    animState.animationTimer += deltaSeconds;
    if (animState.animationTimer >= FRAME_DURATION) {
        animState.animationTimer = 0;
        
        if (animState.currentFrame > NEUTRAL_FRAME) {
            animState.currentFrame--;
        } else if (animState.currentFrame < NEUTRAL_FRAME) {
            animState.currentFrame++;
        }
        update_sprite_frame(drawable->sprite);
    }
}

void Core::update_sprite_frame(sf::Sprite& sprite)
{
    sprite.setTextureRect(sf::IntRect(
        animState.currentFrame * sprite.getTextureRect().width,
        0,
        sprite.getTextureRect().width,
        sprite.getTextureRect().height
    ));
}

void Core::handle_idle_animation(float deltaSeconds, std::optional<component::drawable>& drawable)
{
    if (!drawable) return;
    if (animState.currentFrame > 0) {
        animState.animationTimer += deltaSeconds;
        if (animState.animationTimer >= FRAME_DURATION) {
            animState.animationTimer = 0;
            animState.currentFrame--;
            update_sprite_frame(drawable->sprite);
        }
    }
}

//===================================ANIMATION=================================




void Core::handle_horizontal_movement(float deltaSeconds, std::optional<component::velocity>& vel,
                                    std::optional<component::drawable>& drawable, std::optional<component::position>& pos)
{
    if (!vel) return;
    
    // Handle left movement
    if (keysPressed[sf::Keyboard::Left]) {
        if (pos.value().x < 0) {
            handle_horizontal_stop(vel);
        } else {
            vel->vx = -baseSpeed;
            send_input_if_needed(GameAction::LEFT, inputState.leftSent);
        }
    } else {
        inputState.leftSent = false;
    }

    // Handle right movement 
    if (keysPressed[sf::Keyboard::Right]) {
        if (pos.value().x > 1810) {  // Ajustez cette valeur selon la largeur de votre fenêtre
            handle_horizontal_stop(vel);
        } else {
            vel->vx = baseSpeed;
            send_input_if_needed(GameAction::RIGHT, inputState.rightSent);
        }
    } else {
        inputState.rightSent = false;
    }

    // Handle horizontal stop
    if (!keysPressed[sf::Keyboard::Left] && !keysPressed[sf::Keyboard::Right]) {
        handle_horizontal_stop(vel);
    }
}

void Core::send_input_if_needed(GameAction action, bool& sentFlag)
{
    if (!sentFlag) {
        std::ostringstream messageStream;
        messageStream << encode_action(action) << " " << network->getId();
        network->send(messageStream.str());
        sentFlag = true;
    }
}
void Core::handle_vertical_stop(std::optional<component::velocity>& vel)
{
    if (!vel) return;
    if (vel->vy != 0) {
        std::ostringstream messageStream;
        messageStream << encode_action(GameAction::STOP_Y) << " " << network->getId();
        network->send(messageStream.str());
        vel->vy = 0;
    }
}
void Core::handle_horizontal_stop(std::optional<component::velocity>& vel)
{
    if (!vel) return;
    if (vel->vx != 0) {
        std::ostringstream messageStream;
        messageStream << encode_action(GameAction::STOP_X) << " " << network->getId();
        network->send(messageStream.str());
        vel->vx = 0;
    }
}

void Core::handle_shoot(float deltaSeconds, std::optional<component::position>& pos)
{
    if (!pos) return;

    shootCooldown += deltaSeconds;
    shootCooldown = std::min(shootCooldown / 1.0f, 1.0f);
    shootBar.setSize(sf::Vector2f(70 * shootCooldown, 5));
    shootBar.setPosition(pos.value().x, pos.value().y - 20);

    if (keysPressed[sf::Keyboard::A] && shootCooldown >= 1.0f) {
        send_input_if_needed(GameAction::SHOOT, inputState.shootSent);
        Entity missile =  reg.spawn_entity();
        reg.emplace_component<component::position>(missile, component::position{pos.value().x + 100, pos.value().y});
        reg.emplace_component<component::velocity>(missile, component::velocity{1, 0});
        sf::Sprite sprite = utils.cat("../ressources/sprites/shoot.png");
        sf::IntRect rect(0, 0, sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height);
        reg.emplace_component<component::drawable>(missile, component::drawable{sprite});
        reg.emplace_component<component::controllable>(missile, component::controllable{false});

        shootCooldown = 0.0f;
    } else {
        inputState.shootSent = false;
    }
}

void Core::control_system()
{
    float deltaSeconds = deltaClock.restart().asSeconds();
    auto const &controllables = reg.get_components<component::controllable>();
    auto &velocities = reg.get_components<component::velocity>();
    auto &positions = reg.get_components<component::position>();
    auto &drawables = reg.get_components<component::drawable>();
    for (size_t i = 0; i < controllables.size(); ++i) {
        auto const &controllable = controllables[i];
        auto &vel = velocities[i];
        auto &pos = positions[i];
        auto &drawable = drawables[i];
        if (controllable && vel && drawable && controllable.value().is_controllable && pos) {
            handle_vertical_movement(deltaSeconds, vel, drawable, pos);
            handle_horizontal_movement(deltaSeconds, vel, drawable, pos);
            handle_shoot(deltaSeconds, pos);
        }
    }
}

void Core::setup_position_timer(boost::asio::steady_timer& position_timer)
{
    position_timer.async_wait([this, &position_timer](const boost::system::error_code& ec) {
        if (!ec) {
            Systems::position_system(reg);
            position_timer.expires_at(position_timer.expiry() + std::chrono::milliseconds(1));
            setup_position_timer(position_timer);
        }
    });
}

void Core::update_hud()
{
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
}

void Core::gui_game() {
    load_spaceship();
    sf::Event event;

    menuMusic.stop();
    Game1Music.play();


    position_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::milliseconds(1));
    setup_position_timer(*position_timer_);
    io_thread_ = std::thread([this]() {
            io_context_.run();
    });

    window.setFramerateLimit(fps);
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
                exit (0);
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
        update_hud();
        control_system();
        for (auto& [name, sprite] : sprites_game) {
            sprite.update();
        }
        window.clear();
        for (const auto& name : drawOrder_game) {
            window.draw(sprites_game[name].getSprite());
        }
        sys.draw_system(reg, window);
        window.draw(fpsText);
        window.draw(latencyText);
        window.draw(shootBar);
        window.display();
    }
}
