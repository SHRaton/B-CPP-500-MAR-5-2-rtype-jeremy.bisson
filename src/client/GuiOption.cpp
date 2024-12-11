/*
** EPITECH PROJECT, 2024
** B-CPP-500-MAR-5-2-rtype-jeremy.bisson
** File description:
** GuiOption
*/

#include "Core.hpp"

void Core::gui_option()
{
    sprites_option["back"].setOriginToMiddle();
    sprites_option["back"].setScale({0.2, 0.2});
    sprites_option["back"].setPosition({50, 50});
    sprites_option["none"].setOriginToMiddle();
    sprites_option["none"].setScale({1.5, 1.5});
    sprites_option["none"].setPosition({300, 400});
    sprites_option["deuteranopy"].setOriginToMiddle();
    sprites_option["deuteranopy"].setScale({1.5, 1.5});
    sprites_option["deuteranopy"].setPosition({300, 500});
    sprites_option["tritanopy"].setOriginToMiddle();
    sprites_option["tritanopy"].setScale({1.5, 1.5});
    sprites_option["tritanopy"].setPosition({300, 600});
    sprites_option["protanopy"].setOriginToMiddle();
    sprites_option["protanopy"].setScale({1.5, 1.5});
    sprites_option["protanopy"].setPosition({300, 700});

    sprites_option["is_active"].setOriginToMiddle();
    sprites_option["is_active"].setScale({0.1, 0.1});

    soundBar.setFillColor(sf::Color::Red);
    soundBar.setSize(sf::Vector2f(500, 50));
    soundBar.setPosition(1200, 400);
    soundBarHitBox.setFillColor(sf::Color::White);
    soundBarHitBox.setSize(sf::Vector2f(500, 50));
    soundBarHitBox.setPosition(1200, 400);

    text_option.setFont(font);
    text_option.setCharacterSize(110);
    text_option.setFillColor(sf::Color::White);
    text_option.setPosition(750, 0);
    text_option.setString("Option");
    text_graphics.setFont(font);
    text_graphics.setCharacterSize(80);
    text_graphics.setFillColor(sf::Color::White);
    text_graphics.setPosition(10, 180);
    text_graphics.setString("graphics");
    text_sound.setFont(font);
    text_sound.setCharacterSize(80);
    text_sound.setFillColor(sf::Color::White);
    text_sound.setPosition(1550, 180);
    text_sound.setString("sounds");
    text_sound_general.setFont(font);
    text_sound_general.setCharacterSize(30);
    text_sound_general.setFillColor(sf::Color::White);
    text_sound_general.setPosition(1000, 400);
    text_sound_general.setString("General");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (daltonismType == DaltonismType::NONE) {
                sprites_option["is_active"].setPosition({640, 380});
            } else if (daltonismType == DaltonismType::DEUTERANOPIA) {
                sprites_option["is_active"].setPosition({640, 480});
            } else if (daltonismType == DaltonismType::TRITANOPIA) {
                sprites_option["is_active"].setPosition({640, 580});
            } else if (daltonismType == DaltonismType::PROTANOPIA) {
                sprites_option["is_active"].setPosition({640, 680});
            }

            if (event.type == sf::Event::MouseMoved) {
                sprites_option["back"].setHovered(sprites_option["back"].isMouseOver(window));
                sprites_option["none"].setHovered(sprites_option["none"].isMouseOver(window));
                sprites_option["deuteranopy"].setHovered(sprites_option["deuteranopy"].isMouseOver(window));
                sprites_option["tritanopy"].setHovered(sprites_option["tritanopy"].isMouseOver(window));
                sprites_option["protanopy"].setHovered(sprites_option["protanopy"].isMouseOver(window));
            }

            if (event.type == sf::Event::MouseButtonPressed && 
                event.mouseButton.button == sf::Mouse::Left) {
                if (sprites_option["back"].isMouseOver(window)) {
                    buttonSound_click.play();
                    return;
                }
                if (sprites_option["none"].isMouseOver(window)) {
                    buttonSound_click.play();
                    daltonismType = DaltonismType::NONE;
                    states.shader = nullptr;
                }
                if (sprites_option["deuteranopy"].isMouseOver(window)) {
                    buttonSound_click.play();
                    daltonismType = DaltonismType::DEUTERANOPIA;
                    if (!loadDaltonismShader(daltonismShader, daltonismType)) {
                        throw std::runtime_error("Error when loading Shader for daltonism");
                    }
                    states.shader = &daltonismShader;
                }
                if (sprites_option["tritanopy"].isMouseOver(window)) {
                    buttonSound_click.play();
                    daltonismType = DaltonismType::TRITANOPIA;
                    if (!loadDaltonismShader(daltonismShader, daltonismType)) {
                        throw std::runtime_error("Error when loading Shader for daltonism");
                    }
                    states.shader = &daltonismShader;
                }
                if (sprites_option["protanopy"].isMouseOver(window)) {
                    buttonSound_click.play();
                    daltonismType = DaltonismType::PROTANOPIA;
                    if (!loadDaltonismShader(daltonismShader, daltonismType)) {
                        throw std::runtime_error("Error when loading Shader for daltonism");
                    }
                    states.shader = &daltonismShader;
                }
                if (soundBarHitBox.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    float mouseX = event.mouseButton.x;
                    float barX = soundBar.getPosition().x;
                    float barWidth = soundBarHitBox.getGlobalBounds().width;

                    soundVolume = (mouseX - barX) / barWidth;
                    soundVolume = std::max(0.0f, std::min(1.0f, soundVolume));
                    soundBar.setSize(sf::Vector2f(barWidth * soundVolume, soundBar.getSize().y));
                    menuMusic.setVolume(soundVolume * 100.0);
                    Game1Music.setVolume(soundVolume * 100.0);
                    buttonSound_click.setVolume(soundVolume * 100.0);
                    powerupSound.setVolume(soundVolume * 100.0);
                }
            }
        }

        window.clear();
        renderTexture.clear(sf::Color::Black);

        for (const auto& name : drawOrder_option) {
            renderTexture.draw(sprites_option[name].getSprite());
        }
        renderTexture.draw(soundBarHitBox);
        renderTexture.draw(soundBar);
        renderTexture.draw(text_option);
        renderTexture.draw(text_graphics);
        renderTexture.draw(text_sound);
        renderTexture.draw(text_sound_general);
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