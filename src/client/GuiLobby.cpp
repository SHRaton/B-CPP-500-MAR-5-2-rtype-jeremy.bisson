#include "Core.hpp"

void Core::gui_lobby()
{
    load_spaceship();
    sf::Event event;
    float scrollSpeed = 50.0f;
    float backgroundPos = 0.0f;
    sf::Clock clock;

    background_lobby = utils.cat("../ressources/background/lobby3.png");
    background_lobby2 = utils.cat("../ressources/background/background.png");
    ready = utils.cat("../ressources/background/ready.png");
    utils.setOriginToMiddle(ready);
    ready.setPosition(1420, 675);
    start = utils.cat("../ressources/background/start.png");
    utils.setOriginToMiddle(start);
    start.setPosition(1420, 775);

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
                    send_input_if_needed(GameAction::START, inputState.startSent);
                }
            } else {
                inputState.startSent = false;
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
        auto scale = ready.getGlobalBounds().contains(worldMousePosition) ? 1.1f : 1.0f;
        ready.setScale(scale, scale);
        auto scale2 = start.getGlobalBounds().contains(worldMousePosition) ? 1.1f : 1.0f;
        start.setScale(scale2, scale2);
        handleServerCommands();
        update_hud();
        window.clear();
        renderTexture.clear(sf::Color::Black);
        renderTexture.draw(fpsText);
        renderTexture.draw(latencyText);
        renderTexture.draw(background_lobby2);
        renderTexture.draw(background_lobby);
        renderTexture.draw(ready);
        renderTexture.draw(start);
        int i = 0;
        for (const auto& player : otherPlayers) {
            sf::Text lobbyText;
            lobbyText.setFont(font);
            lobbyText.setCharacterSize(30);
            if (i == network->getId()) {
                lobbyText.setFillColor(sf::Color::Red);
            } else {
                lobbyText.setFillColor(sf::Color::White);
            }
            lobbyText.setPosition(1300, 395 + (50 * i));
            lobbyText.setString("Player " + std::to_string(i + 1));
            renderTexture.draw(lobbyText);
            i++;
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
    delete backgroundTexture;
}