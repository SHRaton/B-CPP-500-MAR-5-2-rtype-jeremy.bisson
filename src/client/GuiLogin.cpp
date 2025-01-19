#include "Core.hpp"
#include "../server/ServerNetwork.hpp"

void Core::handleKeyboard(sf::Event::KeyEvent key)
{
    bool shift = key.shift;
    char letter = utils.keyToAscii(key.code, shift);
    if (select_button == 1) {
        if (key.code == sf::Keyboard::Backspace) {
            if (!str_name.empty()) {
                str_name.pop_back();
                text_name.setString(str_name);
            }
        } else if (letter != '\0') {
            str_name = str_name + letter;
            text_name.setString(str_name);
        }
    }
    if (select_button == 2) {
        if (key.code == sf::Keyboard::Backspace) {
            if (!str_ip.empty()) {
                str_ip.pop_back();
                text_ip.setString(str_ip);
            }
        } else if (letter != '\0') {
            str_ip = str_ip + letter;
            text_ip.setString(str_ip);
        }
    }
    if (select_button == 3) {
        if (key.code == sf::Keyboard::Backspace) {
            if (!str_port.empty()) {
                str_port.pop_back();
                text_port.setString(str_port);
            }
        } else if (letter != '\0') {
            str_port = str_port + letter;
            text_port.setString(str_port);
        }
    }
}

void Core::login()
{
    if (!str_name.empty() && !str_ip.empty() && !str_port.empty()) {
        try {
            initialize_network(str_ip, std::stoi(str_port));
            network->send(encode_action(GameAction::CONNECT) + " " + str_name);
            utils.printLog(str_name + " logged in");
            std::cout << Color::YELLOW << "[Client] Connected to " << Color::BLUE << str_ip << ":" << str_port << Color::RESET << std::endl;
            gui_lobby();
            //gui_game();
        } catch (const std::exception& e) {
            failed_connection = 1;
            str_failed = "Connection failed.";
            text_failed.setString(str_failed);
            std::cerr << e.what() << std::endl;
        }
    } else {
        failed_connection = 1;
        str_failed = "Please fill in all fields";
        text_failed.setString(str_failed);
    }
}

void Core::login_auto(std::string ip, std::string port)
{
    str_name = "Anonymous";
    str_ip = ip;
    str_port = port;
    try {
        initialize_network(str_ip, std::stoi(str_port));
        network->send(encode_action(GameAction::CONNECT) + " " + str_name);
        gui_lobby();
        gui_game();
    } catch (const std::exception& e) {
        std::cerr << "Exception attrapée : " << e.what() << std::endl;
        exit (84);
    }
}

void Core::handleMouseClick(sf::Vector2i mousePosition)
{
    select_button = 0;
    sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition);

    if (sprites_login["connect"].getSprite().getGlobalBounds().contains(worldMousePosition)) {
        buttonSound_click.play();
        login();
    } else if (sprites_login["name"].getSprite().getGlobalBounds().contains(worldMousePosition)) {
        buttonSound_click.play();
        select_button = 1;
    } else if (sprites_login["ip"].getSprite().getGlobalBounds().contains(worldMousePosition)) {
        buttonSound_click.play();
        select_button = 2;
    } else if (sprites_login["port"].getSprite().getGlobalBounds().contains(worldMousePosition)) {
        buttonSound_click.play();
        select_button = 3;
    } else {
        select_button = 0;
    }
}

void Core::gui_login() {
    sprites_login["blueGalaxy"].setScale({2, 2});
    sprites_login["rtype"].setOriginToMiddle();
    sprites_login["rtype"].setScale({1.4, 1.4});
    sprites_login["rtype"].setPosition({960 , 100});
    sprites_login["box_opacity"].setOriginToMiddle();
    sprites_login["box_opacity"].setScale({1.5, 1.5});
    sprites_login["box_opacity"].setPosition({960 , 600});

    sprites_login["name"].setOriginToMiddle();
    sprites_login["name"].setScale({0.8, 0.8});
    sprites_login["name"].setPosition({960, 350});

    sprites_login["ip"].setOriginToMiddle();
    sprites_login["ip"].setScale({0.8, 0.8});
    sprites_login["ip"].setPosition({960, 550});

    sprites_login["port"].setOriginToMiddle();
    sprites_login["port"].setScale({0.8, 0.8});
    sprites_login["port"].setPosition({960, 750});

    sprites_login["connect"].setOriginToMiddle();
    sprites_login["connect"].setScale({0.8, 0.8});
    sprites_login["connect"].setPosition({960, 950});

    text_name.setFont(font);
    text_name.setCharacterSize(35);
    text_name.setFillColor(sf::Color::White);
    text_name.setPosition(550, 365);
    text_name.setString(str_name);

    text_ip.setFont(font);
    text_ip.setCharacterSize(35);
    text_ip.setFillColor(sf::Color::White);
    text_ip.setPosition(550, 565);
    text_ip.setString(str_ip);

    text_port.setFont(font);
    text_port.setCharacterSize(35);
    text_port.setFillColor(sf::Color::White);
    text_port.setPosition(550, 765);
    text_port.setString(str_port);

    text_failed.setFont(font);
    text_failed.setCharacterSize(30);
    text_failed.setFillColor(sf::Color::Red);
    text_failed.setPosition(20, 20);
    text_failed.setString(str_failed);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
                exit (0);
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                std::cout << "Position de la souris : x = " << mousePosition.x << ", y = " << mousePosition.y << std::endl;
                handleMouseClick(mousePosition);
            }
            if (event.type == sf::Event::KeyPressed) {
                handleKeyboard(event.key);
            }
        }
        for (auto& [name, sprite] : sprites_login) {
            sprite.update();
        }
        window.clear();
        renderTexture.clear(sf::Color::Black);
        for (const auto& name : drawOrder_login) {
            renderTexture.draw(sprites_login[name].getSprite());
        }
        renderTexture.draw(text_name);
        renderTexture.draw(text_ip);
        renderTexture.draw(text_port);
        if (failed_connection == 1) {
            renderTexture.draw(text_failed);
        }
        renderTexture.display();
        sf::Sprite screenSprite(renderTexture.getTexture());
        if (daltonismType != DaltonismType::NONE) {
            window.draw(screenSprite, &daltonismShader);
        } else {
            window.draw(screenSprite);
        }
        window.display();
    }
}
