#include "LevelEditor.hpp"

//  Le client doit d'abord envoyer (LEVEL_EDITOR filename)
//Le serveur envoie plusieurs (LEVEL_EDITOR type x y) pour initialiser toutes les entités
//Le client doit les stocker dans un std::vector<EntityPosition>
//Quand c'est fini, le serveur envoie LEVEL_EDITOR (sans arguments)
// et le client crée un LevelEditor avec le std::vector<EntityPosition> en argument

LevelEditor::LevelEditor(std::vector<EntityPosition> entities, CurrentMap current_map, UDPNetworkClient& network) :
    currentMap(current_map),
    network(network)
{
    window.create(sf::VideoMode(1920, 1080), "Level Editor");
    setAssets();
    addAllExistingEntities(entities);
}

LevelEditor::~LevelEditor() {
}

void LevelEditor::addAllExistingEntities(std::vector<EntityPosition> entities)
{
    for (auto& entity : entities) {
        addExistingEntity(entity.type, entity.x, entity.y);
    }
}

void LevelEditor::addExistingEntity(int type, int x, int y)
{
    if (type == 10) {
        sf::Sprite entity = utils.cat("../ressources/sprites/" + currentMap.getMob1());
        entity.setPosition(x, y);
        int frameWidth = currentMap.getMob1Sprite().getGlobalBounds().width / currentMap.getMob1Frames();
        int frameHeight = currentMap.getMob1Sprite().getGlobalBounds().height;
        sf::IntRect rect(0, 0, frameWidth, frameHeight);
        entity.setTextureRect(rect);
        entity.setScale(3, 3);
        entitySprites.push_back(entity);
    } else if (type == 11) {
        sf::Sprite entity = utils.cat("../ressources/sprites/" + currentMap.getMob2());
        entity.setPosition(x, y);
        int frameWidth = currentMap.getMob2Sprite().getGlobalBounds().width / currentMap.getMob2Frames();
        int frameHeight = currentMap.getMob2Sprite().getGlobalBounds().height;
        sf::IntRect rect(0, 0, frameWidth, frameHeight);
        entity.setTextureRect(rect);
        entity.setScale(3, 3);
        entitySprites.push_back(entity);
    } else if (type == 0) {
        sf::Sprite entity = utils.cat("../ressources/sprites/pow1.png");
        entity.setPosition(x, y);
        entitySprites.push_back(entity);
    } else if (type == 1) {
        sf::Sprite entity = utils.cat("../ressources/sprites/pow2.png");
        entity.setPosition(x, y);
        entitySprites.push_back(entity);
    } else if (type == 2) {
        sf::Sprite entity = utils.cat("../ressources/sprites/pow3.png");
        entity.setPosition(x, y);
        entity.setScale(2.5, 2.5);
        entitySprites.push_back(entity);
    } else if (type == 3) {
        sf::Sprite entity = utils.cat("../ressources/sprites/force.png");
        entity.setPosition(x, y);
        sf::IntRect rect(0, 0, entity.getGlobalBounds().width / 12, entity.getGlobalBounds().height);
        entity.setTextureRect(rect);
        entity.setScale(3, 3);
        entitySprites.push_back(entity);
    } else if (type == 50) {
        sf::Sprite entity = utils.cat("../ressources/sprites/meteor.png");
        entity.setPosition(x, y);
    }
}

void LevelEditor::drawAll()
{
    window.clear();
    window.draw(background);
    for (auto& entity : entitySprites) {
        window.draw(entity);
    }

    // Update progress bar width based on scroll position
    float progress = -background.getPosition().x / (background.getGlobalBounds().width - window.getSize().x);
    progress = std::max(0.0f, std::min(1.0f, progress));  // Clamp entre 0 et 1
    progressBar.setSize(sf::Vector2f(300 * progress, 20));
    window.draw(progressBackground);
    window.draw(progressBar);

    window.display();
}

void LevelEditor::setAssets()
{
    background = currentMap.getBackgroundSprite();

    progressBackground.setFillColor(sf::Color(100, 100, 100));
    progressBackground.setSize(sf::Vector2f(300, 20));  // Largeur de base et hauteur
    progressBackground.setPosition(window.getSize().x / 2 - 150, window.getSize().y - 50);

    progressBar.setFillColor(sf::Color::Green);
    progressBar.setSize(sf::Vector2f(300, 20));
    progressBar.setPosition(window.getSize().x / 2 - 150, window.getSize().y - 50);
}

void LevelEditor::updateCurrentMap()
{
    if (levelSelected == 1) {
        currentMap = CurrentMap("level1.json", "Level_1/background.png", "Level_1/mob1.png", 8, "Level_1/mob2.png", 3, "Level_1/boss.png", 8, "Level_1/obstacle.png");
    }
    if (levelSelected == 2) {
        currentMap = CurrentMap("level2.json", "Level_2/background.png", "Level_2/mob1.png", 2, "Level_2/mob2.png", 4, "Level_2/boss.png", 4, "Level_2/obstacle.png");
    }
    if (levelSelected == 3) {
        currentMap = CurrentMap("level3.json", "Level_3/background.png", "Level_3/mob1.png", 6, "Level_3/mob2.png", 3, "Level_3/boss.png", 3, "Level_3/obstacle.png");
    }
}

int LevelEditor::spawnEntity(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            window.close();
        }
        if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
            sf::Sprite entity = utils.cat("../ressources/sprites/" + currentMap.getMob1());
            entity.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            int frameWidth = currentMap.getMob1Sprite().getGlobalBounds().width / currentMap.getMob1Frames();
            int frameHeight = currentMap.getMob1Sprite().getGlobalBounds().height;
            sf::IntRect rect(0, 0, frameWidth, frameHeight);
            entity.setTextureRect(rect);
            entity.setScale(3, 3);
            entitySprites.push_back(entity);
            return 10;
        } else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
            sf::Sprite entity = utils.cat("../ressources/sprites/" + currentMap.getMob2());
            entity.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            int frameWidth = currentMap.getMob2Sprite().getGlobalBounds().width / currentMap.getMob2Frames();
            int frameHeight = currentMap.getMob2Sprite().getGlobalBounds().height;
            sf::IntRect rect(0, 0, frameWidth, frameHeight);
            entity.setTextureRect(rect);
            entity.setScale(3, 3);
            entitySprites.push_back(entity);
            return 11;
        } else if (event.key.code == sf::Keyboard::A) {
            sf::Sprite entity = utils.cat("../ressources/sprites/pow1.png");
            entity.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            entitySprites.push_back(entity);
            return 0;
        } else if (event.key.code == sf::Keyboard::Z) {
            sf::Sprite entity = utils.cat("../ressources/sprites/pow2.png");
            entity.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            entitySprites.push_back(entity);
            return 1;
        } else if (event.key.code == sf::Keyboard::E) {
            sf::Sprite entity = utils.cat("../ressources/sprites/pow3.png");
            entity.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            entity.setScale(2.5, 2.5);
            entitySprites.push_back(entity);
            return 2;
        } else if (event.key.code == sf::Keyboard::R) {
            sf::Sprite entity = utils.cat("../ressources/sprites/force.png");
            entity.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            sf::IntRect rect(0, 0, entity.getGlobalBounds().width / 12, entity.getGlobalBounds().height);
            entity.setTextureRect(rect);
            entity.setScale(3, 3);
            entitySprites.push_back(entity);
            return 3;
        } else if (event.key.code == sf::Keyboard::T) {
            sf::Sprite entity = utils.cat("../ressources/sprites/meteor.png");
            entity.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            entity.setScale(4, 4);
            entitySprites.push_back(entity);
            return 50;
        }
    }
    return -1;
}

std::string LevelEditor::encode_action(GameAction action)
{
    std::bitset<6> bits(static_cast<unsigned long>(action));
    return bits.to_string();
}

void LevelEditor::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            int entity_type = spawnEntity(event);
            if (entity_type != -1) {
                int positionX = sf::Mouse::getPosition(window).x;
                int relativeX = positionX - background.getPosition().x;
                int relativeY = sf::Mouse::getPosition(window).y;
                //send to server LEVEL_EDITOR entity_type, relativeX, relativeY, filename
                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::LEVEL_EDITOR) << " " << entity_type << " " << relativeX << " " << relativeY << " " << currentMap.getFilepath();
                network.send(messageStream.str());
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace) {
                for (auto it = entitySprites.begin(); it != entitySprites.end(); ++it) {
                    if (it->getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
                        int entity_id = it - entitySprites.begin();
                        //send to server LEVEL_EDITOR -1 entity_id filename
                        std::ostringstream messageStream;
                        messageStream << encode_action(GameAction::LEVEL_EDITOR) << " -1 " << entity_id << " " << currentMap.getFilepath();
                        network.send(messageStream.str());
                        entitySprites.erase(it);
                        break;
                    }
                }
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                float newPosition;
                if (event.mouseWheelScroll.delta > 0) {
                    newPosition = background.getPosition().x + 100;
                    // Vérifie si le nouveau déplacement ne dépasse pas la limite gauche (0)
                    if (newPosition <= 0) {
                        background.move(100, 0);
                        for (auto& entity : entitySprites) {
                            entity.move(100, 0);
                        }
                    }
                } else {
                    newPosition = background.getPosition().x - 100;
                    // Vérifie si le nouveau déplacement ne dépasse pas la limite droite
                    // (taille de la fenêtre - taille du background)
                    if (newPosition >= -background.getGlobalBounds().width + window.getSize().x) {
                        background.move(-100, 0);
                        for (auto& entity : entitySprites) {
                            entity.move(-100, 0);
                        }
                    }
                }
            }
        }
        drawAll();
    }
}