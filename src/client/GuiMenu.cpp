#include "Core.hpp"
#include <cmath>

void Core::gui_menu() {
    std::map<std::string, AnimationLobby> animations;
    
    sprites_menu["poudreBleu"].setScale({2, 2});
    
    sprites_menu["rtype"].setOriginToMiddle();
    sprites_menu["rtype"].setScale({1.4, 1.4});
    sprites_menu["rtype"].setPosition({-200, 100});
    animations["rtype"] = {
        0.0f, true,
        960.0f, 100.0f,
        -200.0f, 100.0f
    };
    
    const std::vector<std::string> buttons = {"play", "quit", "option"};
    std::map<std::string, sf::Vector2f> buttonPositions = {
        {"play", {960, 500}},
        {"quit", {960, 800}},
        {"option", {1850, 950}}
    };
    std::map<std::string, sf::Vector2f> buttonScales = {
        {"play", {0.5f, 0.5f}},
        {"quit", {0.5f, 0.5f}},
        {"option", {0.2f, 0.2f}}
    };
    
    for (const auto& button : buttons) {
        sprites_menu[button].setOriginToMiddle();
        sprites_menu[button].setScale(buttonScales[button]);
        sprites_menu[button].setPosition(buttonPositions[button]);
    }

    menuMusic.play();
    sf::Clock animationClock;
    const float animationDuration = 1.0f;
    
    float pulseTime = 0.0f;
    const float pulseSpeed = 3.0f;
    const float pulseAmount = 0.05f;

    while (window.isOpen()) {
        sf::Time deltaTime = animationClock.restart();
        float dt = deltaTime.asSeconds();
        pulseTime += dt;
        
        for (auto& [name, anim] : animations) {
            if (anim.isAnimating) {
                anim.progress = std::min(1.0f, anim.progress + dt / animationDuration);
                float easedProgress = 1.0f - std::pow(1.0f - anim.progress, 3.0f);
                
                float currentX = anim.startX + (anim.targetX - anim.startX) * easedProgress;
                float currentY = anim.startY + (anim.targetY - anim.startY) * easedProgress;
                
                sprites_menu[name].setPosition({currentX, currentY});
                
                if (anim.progress >= 1.0f) {
                    anim.isAnimating = false;
                }
            }
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (event.type == sf::Event::MouseMoved) {
                for (const auto& button : buttons) {
                    bool isHovered = sprites_menu[button].isMouseOver(window);
                    sprites_menu[button].setHovered(isHovered);
                    
                    if (isHovered) {
                        sprites_menu[button].setScale({
                            buttonScales[button].x * 1.1f,
                            buttonScales[button].y * 1.1f
                        });
                    } else {
                        sprites_menu[button].setScale(buttonScales[button]);
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && 
                event.mouseButton.button == sf::Mouse::Left) {
                if (sprites_menu["play"].isMouseOver(window)) {
                    buttonSound_click.play();
                    gui_login();
                }
                if (sprites_menu["quit"].isMouseOver(window)) {
                    buttonSound_click.play();
                    window.close();
                }
                if (sprites_menu["option"].isMouseOver(window)) {
                    buttonSound_click.play();
                    gui_option();
                }
            }
        }

        float pulseFactor = 1.0f + (std::sin(pulseTime * pulseSpeed) * pulseAmount);
        for (const auto& button : buttons) {
            if (!sprites_menu[button].isMouseOver(window)) {
                sprites_menu[button].setScale({
                    buttonScales[button].x * pulseFactor,
                    buttonScales[button].y * pulseFactor
                });
            }
        }

        for (auto& [name, sprite] : sprites_menu) {
            if (name == "blueGalaxy") {
                sprite.update_xy(1920, 0);
            } else {
                sprite.update();
            }
        }

        window.clear();
        renderTexture.clear(sf::Color::Black);
        for (const auto& name : drawOrder_menu) {
            renderTexture.draw(sprites_menu[name].getSprite());
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