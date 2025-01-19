#include "Core.hpp"

void Core::initializeLevelSelector()
{
    int x = 270, y = 190;
    int size_x = 400, size_y = 80;

    // Configuration du bouton principal du menu déroulant
    dropdownButton.setSize(sf::Vector2f(size_x, size_y));
    dropdownButton.setPosition(x, y);
    dropdownButton.setFillColor(sf::Color(40, 40, 40));

    // Configuration du texte principal avec un petit décalage par rapport au bouton
    dropdownText.setFont(font);
    dropdownText.setCharacterSize(size_y/2);
    dropdownText.setFillColor(sf::Color::White);
    dropdownText.setPosition(x + 10, y + 10);  // Décalage de 10 pixels pour le texte
    dropdownText.setString("Level " + std::to_string(levelSelected));

    // Vider les vecteurs au cas où
    levelButtons.clear();
    levelTexts.clear();

    // Création des boutons et textes pour chaque niveau
    for (size_t i = 0; i < levelFiles.size(); i++) {
        // Création du bouton
        sf::RectangleShape button(sf::Vector2f(400, 80));
        button.setPosition(x, y + size_y + i * size_y + 2);  // Position relative au bouton principal
        button.setFillColor(sf::Color(25, 25, 25));
        levelButtons.push_back(button);

        // Création du texte
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(size_y/2);
        text.setFillColor(sf::Color::White);
        text.setPosition(x + 10, y + size_y + 10 + i * size_y + 2);  // Position relative au bouton avec décalage

        // Extraire juste le nom du niveau du chemin complet
        std::string levelName = levelFiles[i];
        size_t lastSlash = levelName.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            levelName = levelName.substr(lastSlash + 1);
        }
        // Enlever l'extension .json
        size_t lastDot = levelName.find_last_of(".");
        if (lastDot != std::string::npos) {
            levelName = levelName.substr(0, lastDot);
        }

        text.setString(levelName);
        levelTexts.push_back(text);
    }
}

void Core::updateLevelSelector()
{
    if (dropdownButton.getGlobalBounds().contains(worldMousePosition)) {
        dropdownButton.setFillColor(sf::Color(40, 40, 40));
    } else {
        dropdownButton.setFillColor(sf::Color(25, 25, 25));
    }
    dropdownText.setString("Level " + std::to_string(levelSelected));

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (dropdownButton.getGlobalBounds().contains(worldMousePosition)) {
            if (!wasMousePressed) {
                isDropdownOpen = !isDropdownOpen;
                buttonSound_click.play();
            }
        } else if (isDropdownOpen) {
            for (size_t i = 0; i < levelButtons.size(); i++) {
                if (levelButtons[i].getGlobalBounds().contains(worldMousePosition)) {
                    levelSelected = i + 1;
                    updateCurrentMap();
                    isDropdownOpen = false;
                    buttonSound_click.play();
                    break;
                }
            }
        }
    }

    // Met à jour l'apparence des boutons de niveau si le menu est ouvert
    if (isDropdownOpen) {
        for (size_t i = 0; i < levelButtons.size(); i++) {
            if (levelButtons[i].getGlobalBounds().contains(worldMousePosition)) {
                levelButtons[i].setFillColor(sf::Color(90, 90, 90));
            } else {
                levelButtons[i].setFillColor(sf::Color(50, 50, 50));
            }
        }
    }
    wasMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

void Core::display_lobby()
{
    renderTexture.clear(sf::Color::Black);
    renderTexture.draw(fpsText);
    renderTexture.draw(latencyText);
    renderTexture.draw(background_lobby2);
    renderTexture.draw(background_lobby);
    updateLevelSelector();
    renderTexture.draw(dropdownButton);
    renderTexture.draw(dropdownText);
    if (isDropdownOpen) {
        for (size_t i = 0; i < levelButtons.size(); i++) {
            renderTexture.draw(levelButtons[i]);
            renderTexture.draw(levelTexts[i]);
        }
    }
    //renderTexture.draw(ready);
    renderTexture.draw(start);
    renderTexture.draw(replay);
    renderTexture.draw(level_editor);
    renderTexture.draw(nb_players_text);
    int playerCount = 0;
    for (const auto& player : otherPlayers) {
        sf::Text lobbyText;
        lobbyText.setFont(font);
        lobbyText.setCharacterSize(25);
        if (playerCount == network->getId()) {
            lobbyText.setFillColor(sf::Color::Red);
        } else {
            lobbyText.setFillColor(sf::Color::White);
        }
        lobbyText.setPosition(1300, 395 + (50 * playerCount));
        lobbyText.setString(otherPlayers[playerCount].name);
        renderTexture.draw(lobbyText);
        if (playerCount < shipSprites.size()) {
            renderTexture.draw(shipSprites[playerCount]);
        }
        playerCount++;
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
}

void Core::load_levels()
{
    network->send(encode_action(GameAction::GET_LEVELS));
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    std::string message = network->receive().value_or("");
    if (message.empty()) {
        throw std::runtime_error("Erreur de connexion au serveur!");
    }
    std::istringstream iss(message);
    std::string code;
    iss >> code;
    if (message.rfind(encode_action(GameAction::GET_LEVELS), 0) == 0) {
        handleGetLevelsCommand(iss);
    }
}

void Core::load_spaceship()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::vector<std::string> messages = network->receive_all();
    if (messages.empty()) {
        throw std::runtime_error("Erreur de connexion au serveur!");
    }

    // Handle multiple player connections
    int i = 0;
    for (const auto& buffer : messages) {
        std::cout << "Mess: " << buffer << "\n";
        if (messages[i].rfind(encode_action(GameAction::HIGHSCORE), 0) == 0) {
            std::cout << "RECUP ET STOCKER ICI";
        }
        if (messages[i].rfind("OK", 0) == 0) {
            break;
        }
        if (buffer.rfind(encode_action(GameAction::CONNECT), 0) == 0) {
            nb_player++;
            std::istringstream iss(buffer);
            std::string code, name;
            int id;
            iss >> code >> id >> name;

            auto newPlayer = reg.spawn_entity();
            std::cout << "Créer AUTRE sprite " << id << std::endl;

            sf::Sprite vaisseau = utils.cat("../ressources/sprites/vaisseau" + std::to_string(id) + ".png");
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
            playerInfo.name = name;
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
        playerInfo.name = str_name;
        playerInfo.hp = 100;
        playerInfo.hpText.setFont(font);
        playerInfo.hpText.setCharacterSize(35);
        playerInfo.hpText.setFillColor(sf::Color::Blue);
        playerInfo.hpText.setPosition(1500, 10 + otherPlayers.size() * 50);
        playerInfo.hpText.setString("Player " + std::to_string(player) + ": " + std::to_string(playerInfo.hp));
        otherPlayers.push_back(playerInfo);
    }
}

void Core::gui_lobby()
{
    // Recup the list of all levels
    load_spaceship();
    load_levels();

    initializeLevelSelector();
    sf::Event event;
    float scrollSpeed = 50.0f;
    float backgroundPos = 0.0f;
    sf::Clock clock;

    nb_players_text.setFont(font);
    nb_players_text.setCharacterSize(30);
    nb_players_text.setFillColor(sf::Color::White);
    nb_players_text.setPosition(1490, 340);
    nb_players_text.setString(std::to_string(nb_player) + " / 4");

    // Chargement des textures de base
    background_lobby = utils.cat("../ressources/background/lobby3.png");
    background_lobby2 = utils.cat("../ressources/background/background.png");
    // ready = utils.cat("../ressources/background/ready.png");
    // utils.setOriginToMiddle(ready);
    // ready.setPosition(1420, 675);
    start = utils.cat("../ressources/background/start.png");
    utils.setOriginToMiddle(start);
    start.setPosition(1420, 775);
    replay = utils.cat("../ressources/background/replay.png");
    utils.setOriginToMiddle(replay);
    replay.setPosition(1420, 675);
    level_editor = utils.cat("../ressources/background/level_editor.png");
    utils.setOriginToMiddle(level_editor);
    level_editor.setPosition(1420, 240);

    std::vector<sf::Vector2f> shipPositions = {
        {1500, 400},  // Position vaisseau 1
        {1500, 450},  // Position vaisseau 2
        {1500, 500},  // Position vaisseau 3
        {1500, 550}  // Position vaisseau 4
    };

    for (int i = 0; i < 4; i++) {
        sf::Sprite vaisseau = utils.cat("../ressources/sprites/vaisseau" + std::to_string(i) + ".png");
        vaisseau.setTextureRect(sf::IntRect(68, 0, 32, 32));
        vaisseau.setPosition(shipPositions[i]);
        vaisseau.setScale(2.0f, 2.0f);
        shipSprites.push_back(vaisseau);
    }

    sf::Texture* backgroundTexture = new sf::Texture();
    if (backgroundTexture->loadFromFile("../ressources/background/background.png")) {
        backgroundTexture->setRepeated(true);
        background_lobby2.setTexture(*backgroundTexture);
    }

    registryWindow.create(sf::VideoMode(1400, 1080), "ECS Debuger");

    while (window.isOpen() && registryWindow.isOpen()) {
        sf::Time elapsed = clock.restart();
        mousePosition = sf::Mouse::getPosition(window);
        worldMousePosition = window.mapPixelToCoords(mousePosition);

        backgroundPos += scrollSpeed * elapsed.asSeconds();
        if (backgroundPos >= backgroundTexture->getSize().x) {
            backgroundPos = 0.0f;
        }
        background_lobby2.setTextureRect(sf::IntRect(static_cast<int>(backgroundPos), 0,
            window.getSize().x, window.getSize().y));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::DISCONNECT) << " " << network->getId();
                network->send(messageStream.str());
                delete backgroundTexture;
                window.close();
                exit (0);
            }
            if (event.type == sf::Event::KeyPressed) {
                keysPressed[event.key.code] = true;
            }
            if (event.type == sf::Event::KeyReleased) {
                keysPressed[event.key.code] = false;
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (start.getGlobalBounds().contains(worldMousePosition)) {
                    buttonSound_click.play();
                    std::ostringstream messageStream;
                    messageStream << encode_action(GameAction::START) << " " << levelSelected;
                    network->send(messageStream.str());
                    send_input_if_needed(GameAction::START, inputState.startSent);
                }
                if (replay.getGlobalBounds().contains(worldMousePosition)) {
                    reg.kill_entity(Entity(0));
                    buttonSound_click.play();
                    isReplay = true;
                    send_input_if_needed(GameAction::PLAY_REPLAY, inputState.startSent);
                }
            } else {
                inputState.startSent = false;
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (level_editor.getGlobalBounds().contains(worldMousePosition)) {
                    if (!inputState.levelEditorSent) {
                        buttonSound_click.play();
                        std::ostringstream messageStream;
                        messageStream << encode_action(GameAction::LEVEL_EDITOR) << " " << currentMap.getFilepath();
                        std::cout << "msg ---> " << messageStream.str() << std::endl;
                        network->send(messageStream.str());
                        inputState.levelEditorSent = true;
                    }
                }
            } else {
                inputState.levelEditorSent = false;
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

        //auto scale = ready.getGlobalBounds().contains(worldMousePosition) ? 1.1f : 1.0f;
        //ready.setScale(scale, scale);
        auto scale2 = start.getGlobalBounds().contains(worldMousePosition) ? 1.1f : 1.0f;
        start.setScale(scale2, scale2);
        auto scale3 = replay.getGlobalBounds().contains(worldMousePosition) ? 1.1f : 1.0f;
        replay.setScale(scale3, scale3);
        auto scale4 = level_editor.getGlobalBounds().contains(worldMousePosition) ? 1.1f : 1.0f;
        level_editor.setScale(scale4, scale4);
        nb_players_text.setString(std::to_string(nb_player) + " / 4");

        handleServerCommands();

        // Window Lobby
        window.clear();
        display_lobby();
        window.display();

        // Window ECS Debug
        registryWindow.clear();
        displayRegistryInfo();
        registryWindow.display();
    }
    delete backgroundTexture;
}