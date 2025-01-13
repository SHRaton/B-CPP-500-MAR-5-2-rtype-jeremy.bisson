#include "Core.hpp"
#include "../Systems.hpp"
#include "../server/ServerNetwork.hpp"

std::string Core::encode_action(GameAction action)
{
    std::bitset<5> bits(static_cast<unsigned long>(action));
    return bits.to_string();
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
            reg.emplace_component<component::health>(newPlayer, component::health{100});
            reg.emplace_component<component::invincible>(newPlayer, component::invincible{false});
            reg.emplace_component<component::type>(newPlayer, component::type{667});
            PlayerInfo playerInfo;
            playerInfo.isReady = false;
            playerInfo.id = id;
            playerInfo.hp = 100;
            playerInfo.hpText.setFont(font);
            playerInfo.hpText.setCharacterSize(35);
            playerInfo.hpText.setFillColor(sf::Color::Blue);
            playerInfo.hpText.setPosition(1500, 10 + otherPlayers.size() * 50);
            playerInfo.hpText.setString("Player " + std::to_string(id) + ": " + std::to_string(playerInfo.hp));
            otherPlayers.push_back(playerInfo);
        }
        i++;
    }
    if (!messages.empty()) {
        // Création du joueur
        player = reg.spawn_entity();
        nb_player++;
        network->setId(player);
        std::cout << "Créer NOTRE sprite " << player << std::endl;
        reg.emplace_component<component::position>(player, component::position{200, 500});
        reg.emplace_component<component::velocity>(player, component::velocity{0, 0});
        reg.emplace_component<component::health>(player, component::health{100});
        sf::Sprite sprite = utils.cat("../ressources/sprites/vaisseau" + std::to_string(player) + ".png");
        sf::IntRect rect(0, 0, sprite.getGlobalBounds().width / 5, sprite.getGlobalBounds().height);
        sprite.setTextureRect(rect);
        sprite.setScale(3, 3);
        reg.emplace_component<component::drawable>(player, component::drawable{sprite});
        reg.emplace_component<component::controllable>(player, component::controllable{true});
        reg.emplace_component<component::invincible>(player, component::invincible{false});
        reg.emplace_component<component::type>(player, component::type{696});
        PlayerInfo playerInfo;
        playerInfo.isReady = false;
        playerInfo.id = player;
        playerInfo.hp = 100;
        playerInfo.hpText.setFont(font);
        playerInfo.hpText.setCharacterSize(35);
        playerInfo.hpText.setFillColor(sf::Color::Blue);
        playerInfo.hpText.setPosition(1500, 10 + otherPlayers.size() * 50);
        playerInfo.hpText.setString("Player " + std::to_string(player) + ": " + std::to_string(playerInfo.hp));
        otherPlayers.push_back(playerInfo);
    }
}


void Core::setup_position_timer(boost::asio::steady_timer& position_timer)
{
    position_timer.async_wait([this, &position_timer](const boost::system::error_code& ec) {
        if (!ec) {
            Systems::position_system(reg);
            position_timer.expires_at(position_timer.expiry() + std::chrono::milliseconds(10));
            setup_position_timer(position_timer);
        }
    });
}

void Core::checkInvincibility()
{
    auto& invincibles = reg.get_components<component::invincible>();
    auto& drawables = reg.get_components<component::drawable>();
    auto now = std::chrono::steady_clock::now();

    for (size_t i = 0; i < invincibles.size(); ++i) {
        if (invincibles[i].value().is_invincible && now >= invincibles[i].value().expiration_time) {
            invincibles[i].value().is_invincible = false;
            drawables[i].value().sprite.setColor(sf::Color::White);
        }
    }
}

void Core::displayRegistryInfo()
{
    auto& positions = reg.get_components<component::position>();
    auto& velocities = reg.get_components<component::velocity>();
    auto& healths = reg.get_components<component::health>();
    auto& drawables = reg.get_components<component::drawable>();
    auto& controllables = reg.get_components<component::controllable>();
    auto& invincibles = reg.get_components<component::invincible>();
    auto& damages = reg.get_components<component::damage>();
    auto& types = reg.get_components<component::type>();

    float yPosition = 50.0f; // Position initiale en Y pour le texte

    for (size_t i = 0; i < positions.size(); ++i) {
        sf::Color entityColor = sf::Color::White; // Couleur par défaut

        std::ostringstream oss;
        oss << "Entity " << i;
        if (types.size() > i && types[i]) {
            switch (types[i].value().type) {
                case 667: // Vaisseau
                    oss << "( Other SpaceShip ) :\n";
                    entityColor = sf::Color::Green;
                    break;
                case 696: // Vaisseau
                    oss << "( My SpaceShip ) :\n";
                    entityColor = sf::Color::Blue;
                    break;
                case 10: // Mob
                    oss << "( Mob 1 ) :\n";
                    entityColor = sf::Color::Red;
                    break;
                case 11: // Mob
                    oss << "( Mob 2 ) :\n";
                    entityColor = sf::Color::Red;
                    break;
                case 0: // PowerUp
                    oss << "( PowerUp 1 ) :\n";
                    entityColor = sf::Color::White;
                    break;
                case 1: // PowerUp
                    oss << "( PowerUp 2 ) :\n";
                    entityColor = sf::Color::White;
                    break;
                case 6: // Missile
                    oss << "( Bullet ) :\n";
                    entityColor = sf::Color::Yellow;
                    break;
                case 7: // Mob Bullet
                    oss << "( Mob Bullet ) :\n";
                    entityColor = sf::Color::Magenta;
                    break;
                default:
                    oss << "( Unknown ):\n";
                    entityColor = sf::Color::White;
                    break;
            }
        }

        if (positions[i]) {
            oss << "  Position: (" << positions[i].value().x << ", " << positions[i].value().y << ")  |  ";
        }
        if (velocities.size() > i && velocities[i]) {
            oss << "  Velocity: (" << velocities[i].value().vx << ", " << velocities[i].value().vy << ")  |  ";
        }
        if (healths.size() > i && healths[i]) {
            oss << "  Health: " << healths[i].value().hp << "  |  ";
        }
        if (drawables.size() > i && drawables[i]) {
            oss << "  Drawable: Yes  |  ";
        }
        if (controllables.size() > i && controllables[i]) {
            oss << "  Controllable: " << (controllables[i].value().is_controllable ? "Yes" : "No") << "  |  ";
        }
        if (invincibles.size() > i && invincibles[i]) {
            oss << "  Invincible: " << (invincibles[i].value().is_invincible ? "Yes" : "No") << "  |  ";
        }
        if (damages.size() > i && damages[i]) {
            oss << "  Damage: " << damages[i].value().dmg << "  |  ";
        }
        oss << "\n";

        // Créer un texte SFML pour cette entité
        sf::Text entityInfo;
        entityInfo.setFont(font);
        entityInfo.setCharacterSize(20);
        entityInfo.setFillColor(sf::Color::Black);
        entityInfo.setPosition(10, yPosition);
        entityInfo.setString(oss.str());

        float textHeight = entityInfo.getLocalBounds().height + 10.0f;
        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(registryWindow.getSize().x, textHeight));
        rect.setFillColor(entityColor);
        rect.setPosition(10, yPosition);


        // Dessiner le texte dans la deuxième fenêtre
        registryWindow.draw(rect);
        registryWindow.draw(entityInfo);


        // Mettre à jour la position Y pour le prochain texte
        yPosition += textHeight;
    }
    sf::Text nbPlayer;
    nbPlayer.setFont(font);
    nbPlayer.setCharacterSize(25);
    nbPlayer.setFillColor(sf::Color::White);
    nbPlayer.setPosition(10, 10);
    nbPlayer.setString("Number of player : " + std::to_string(nb_player));
    registryWindow.draw(nbPlayer);

    sf::Text myId;
    myId.setFont(font);
    myId.setCharacterSize(25);
    myId.setFillColor(sf::Color::White);
    myId.setPosition(400, 10);
    myId.setString("My ID : " + std::to_string(network->getId()));
    registryWindow.draw(myId);
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
    if (scoreScale > 1.0f) {
        float deltaTime = scoreAnimClock.restart().asSeconds();
        scoreScale -= deltaTime * 2.0f;
        
        if (scoreScale < 1.0f) {
            scoreScale = 1.0f;
            globalScore_text.setFillColor(sf::Color::White);
        }
        
        globalScore_text.setScale(scoreScale, scoreScale);
    }

    updateAnimations();
}

void Core::gui_gamewin() {
    if (!gameWinTexture.loadFromFile("../ressources/sprites/win.png")) {
        throw std::runtime_error("Error loading game win texture");
    }
    gameWinSprite.setTexture(gameWinTexture);
    Game1Music.stop();
    winMusic.play();
    
    // Centrer l'image
    gameWinSprite.setPosition(
        window.getSize().x / 2 - gameWinSprite.getGlobalBounds().width / 2,
        window.getSize().y / 2 - gameWinSprite.getGlobalBounds().height / 2
    );

    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::DISCONNECT) << " " << network->getId();
                network->send(messageStream.str());
                window.close();
                exit(0);
            }
        }

        window.clear(sf::Color::Black);
        window.draw(gameWinSprite);
        window.draw(save_replay);
        window.display();
    }
}


void Core::gui_gameover() {
    if (!gameOverTexture.loadFromFile("../ressources/sprites/lose.png")) {
        throw std::runtime_error("Error loading game over texture");
    }
    gameOverSprite.setTexture(gameOverTexture);
    Game1Music.stop();
    gameoverMusic.play();
    
    // Centrer l'image
    gameOverSprite.setPosition(
        window.getSize().x / 2 - gameOverSprite.getGlobalBounds().width / 2,
        window.getSize().y / 2 - gameOverSprite.getGlobalBounds().height / 2
    );

    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)) {
                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::DISCONNECT) << " " << network->getId();
                network->send(messageStream.str());
                window.close();
                exit(0);
            }
        }

        window.clear(sf::Color::Black);
        window.draw(gameOverSprite);
        window.draw(save_replay);
        window.display();
    }
}

void Core::display_all()
{
    window.clear();
    renderTexture.clear(sf::Color::Black);
    if (isDead) {
        states.shader = &blackAndWhiteShader;
    } else {
        states.shader = nullptr;
    }
    for (const auto& name : drawOrder_game) {
        renderTexture.draw(sprites_game[name].getSprite());
    }
    sys.draw_system(reg, renderTexture);
    renderTexture.draw(fpsText);
    renderTexture.draw(latencyText);
    renderTexture.draw(globalScore_text);
    if (!isDead) {
        renderTexture.draw(shootBar);
        renderTexture.draw(superShootBar);
        if (laserActive) {
            if (laserClock.getElapsedTime().asSeconds() <= 4.95) {
                renderTexture.draw(laserPowerUpLogo);
            } else {
                laserActive = false;
            }
        }
    }
    for (const auto& player : otherPlayers) {
        renderTexture.draw(player.hpText);
    }
    if (isDead) {
        sf::Text deadText;
        deadText.setFont(font);
        deadText.setCharacterSize(50);
        deadText.setFillColor(sf::Color::Red);
        deadText.setPosition(window.getSize().x - 450, window.getSize().y - 80);
        deadText.setString("You are Dead");
        renderTexture.draw(deadText);
    }
    renderTexture.display();
    sf::Sprite screenSprite(renderTexture.getTexture());
    if (daltonismType != DaltonismType::NONE && !isDead) {
        window.draw(screenSprite, &daltonismShader);
    } else if (isDead) {
        window.draw(screenSprite, states);
    } else {
        window.draw(screenSprite);
    }
    window.display();
    registryWindow.clear();
    displayRegistryInfo();
    registryWindow.display();
}

void Core::gui_game()
{
    loadAssetsGame();
    save_replay.setScale(0.1, 0.1);
    save_replay.setPosition(1800, 50);
    sf::Event event;

    menuMusic.stop();
    Game1Music.play();

    registryWindow.create(sf::VideoMode(1400, 1080), "ECS Debuger");

    position_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::milliseconds(2));
    setup_position_timer(*position_timer_);
    io_thread_ = std::thread([this]() {
            io_context_.run();
    });

    if (daltonismType != DaltonismType::NONE) {
        if (!loadDaltonismShader(daltonismShader, daltonismType)) {
            throw std::runtime_error("Error when loading Shader for daltonism");
        }
    }
    if (!loadBlackAndWhiteShader()) {
        throw std::runtime_error("Error when loading black and white shader");
    }
    playIntroAnimation();
    while (window.isOpen() && registryWindow.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::DISCONNECT) << " " << network->getId();
                network->send(messageStream.str());
                window.close();
                exit (0);
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
                if (save_replay.getGlobalBounds().contains(worldPos)) {
                    buttonSound_click.play();
                    std::ostringstream messageStream;
                    messageStream << encode_action(GameAction::SAVE_REPLAY);
                    network->send(messageStream.str());
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                keysPressed[event.key.code] = true;
            }
            if (event.type == sf::Event::KeyReleased) {
                keysPressed[event.key.code] = false;
            }
        }
        while (registryWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::DISCONNECT) << " " << network->getId();
                network->send(messageStream.str());
                window.close();
                registryWindow.close();
                exit (0);
            }
        }
        
        sf::Vector2f background2Pos = sprites_game["background_game2"].getSprite().getPosition();
        if (background2Pos.x <= -15100) {
            gui_gamewin();
            return;
        }

        handleServerCommands();
        update_hud();
        control_system();
        checkInvincibility();
        for (auto& [name, sprite] : sprites_game) {
            sprite.update();
        }
        display_all();
    }
}