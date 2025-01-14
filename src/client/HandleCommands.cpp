/*
** EPITECH PROJECT, 2024
** B-CPP-500-MAR-5-2-rtype-jeremy.bisson
** File description:
** HandleCommands
*/

#include "Core.hpp"
#include "../Systems.hpp"
#include "../server/ServerNetwork.hpp"

void Core::handleServerCommands()
{
    network->print_message_queue();
    buffer = network->receive().value_or("");

    if (buffer.empty()) return;

    std::istringstream iss(buffer);
    std::string code;
    iss >> code;

    if (code == encode_action(GameAction::MOVE)) {
        handleMoveCommand(iss);
    }
    else if (code == encode_action(GameAction::MOB_SPAWN)) {
        handleMobSpawnCommand(iss);
    }
    else if (code == encode_action(GameAction::CONNECT)) {
        handleConnectCommand(iss);
    }
    else if (code == encode_action(GameAction::DISCONNECT)) {
        handleDisconnectCommand(iss);
    }
    else if (code == encode_action(GameAction::UP) || 
             code == encode_action(GameAction::DOWN) ||
             code == encode_action(GameAction::LEFT) ||
             code == encode_action(GameAction::RIGHT) ||
             code == encode_action(GameAction::STOP_X) ||
             code == encode_action(GameAction::STOP_Y)) {
        handleMovementCommands(iss, static_cast<GameAction>(std::bitset<5>(code).to_ulong()));
    }
    else if (code == encode_action(GameAction::SHOOT)) {
        handleShootCommands(iss);
    }
    else if (code == encode_action(GameAction::SUPER_SHOOT)) {
        handleSuperShootCommands(iss);
    }
    else if (code == encode_action(GameAction::LASER_SHOOT)) {
        handleLaserShootCommands(iss);
    }
    else if (code == encode_action(GameAction::MOB_SHOOT)) {
        handleMobShootCommand(iss);
    }
    else if (code == encode_action(GameAction::ENTITY_SPAWN)) {
        handlePowerUpCommand(iss);
    }
    else if (code == encode_action(GameAction::COLLISION)) {
        handleCollisionCommand(iss);
    }
    else if (code == encode_action(GameAction::DEATH)) {
        handleDeathCommand(iss);
    }
    else if (code == encode_action(GameAction::WIN)) {
        handleWinCommand(iss);
    }
    else if (code == encode_action(GameAction::START)) {
        handleStartCommand(iss);
    }
    else if (code == encode_action(GameAction::SCORE_UPDATE)) {
        handleScoreUpdateCommand(iss);
    }
    else if (code == encode_action(GameAction::LOOSE)) {
        handleLooseCommand(iss);
    }
    else if (code == encode_action(GameAction::LEVEL_EDITOR)) {
        handleLevelEditorCommand(iss);
    }
    else {
        std::cout << "Commande inconnue : " << buffer << std::endl;
    }
}

void Core::handleLooseCommand(std::istringstream& iss)
{
    gui_gameover();
}

void Core::handleLevelEditorCommand(std::istringstream& iss)
{
    std::string fullMessage;
    std::getline(iss, fullMessage);
    std::istringstream messageStream(fullMessage);

    levelFiles.clear();
    std::string file;
    while (messageStream >> file) {
        levelFiles.push_back(file);
    }
}

void Core::handleMoveCommand(std::istringstream& iss)
{
    int id, x, y;
    iss >> id >> x >> y;

    auto& positions = reg.get_components<component::position>();
    if (id >= 0 && id < positions.size() && positions[id]) {
        positions[id].value().x = x;
        positions[id].value().y = y;
    } else {
        std::cout << "Erreur : ID de l'Entitée invalide." << std::endl;
    }
}

void Core::handleScoreUpdateCommand(std::istringstream& iss)
{
    int id, score;
    iss >> id >> score;

    if (score > globalScore) { 
        scoreScale = 1.3f;
        globalScore_text.setFillColor(sf::Color::Yellow);
        scoreAnimClock.restart();
    }

    globalScore = score;
    globalScore_text.setString("Score : " + std::to_string(globalScore));
    globalScore_text.setScale(scoreScale, scoreScale);
}

void Core::handleMobSpawnCommand(std::istringstream& iss)
{
    int mob_type, x, y;
    iss >> mob_type >> x >> y;

    auto newMob = reg.spawn_entity();
    std::string mob_path;
    if (mob_type == 0) {
        mob_path = currentMap.getMob1();
    } else if (mob_type == 1) {
        mob_path = currentMap.getMob2();
    }
    sf::Sprite mob = utils.cat("../ressources/sprites/" + mob_path);
    mob.setPosition(x, y);
    
    reg.emplace_component<component::position>(newMob, component::position{x, y});
    //red mob
    if (mob_type == 0) {
        int frameWidth = currentMap.getMob1Sprite().getGlobalBounds().width / currentMap.getMob1Frames();
        int frameHeight = currentMap.getMob1Sprite().getGlobalBounds().height;
        sf::IntRect rect(0, 0, frameWidth, frameHeight);
        mob.setTextureRect(rect);
        
        reg.emplace_component<component::health>(newMob, component::health{300});
        reg.emplace_component<component::damage>(newMob, component::damage{10});
        reg.emplace_component<component::velocity>(newMob, component::velocity{-5, 0});
        reg.emplace_component<component::type>(newMob, component::type{10});
        reg.emplace_component<component::animation>(newMob, component::animation{
            0, currentMap.getMob1Frames(), 0.2f,
            sf::Clock()
        });
    } else if (mob_type == 1) {
        // Grey mob
        int frameWidth = currentMap.getMob2Sprite().getGlobalBounds().width / currentMap.getMob2Frames();
        int frameHeight = currentMap.getMob2Sprite().getGlobalBounds().height;
        sf::IntRect rect(0, 0, frameWidth, frameHeight);
        mob.setTextureRect(rect);
        
        reg.emplace_component<component::health>(newMob, component::health{100});
        reg.emplace_component<component::damage>(newMob, component::damage{40});
        reg.emplace_component<component::velocity>(newMob, component::velocity{-5, 0});
        reg.emplace_component<component::type>(newMob, component::type{11});
        reg.emplace_component<component::animation>(newMob, component::animation{
            0, currentMap.getMob2Frames(), 0.3f,
            sf::Clock()
        });
    }
    mob.setScale(3, 3);
    reg.emplace_component<component::drawable>(newMob, component::drawable{mob});
}

void Core::handleConnectCommand(std::istringstream& iss)
{
    int id;
    iss >> id;

    nb_player++;
    joiningSound.play();
    auto newPlayer = reg.spawn_entity();
    sf::Sprite vaisseau = utils.cat("../ressources/sprites/vaisseau" + std::to_string(id) + ".png");

    vaisseau.setPosition(200, 500);
    sf::IntRect rect(0, 0, vaisseau.getGlobalBounds().width / 5, vaisseau.getGlobalBounds().height);
    vaisseau.setTextureRect(rect);
    vaisseau.setScale(3, 3);

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

void Core::handleMovementCommands(std::istringstream& iss, GameAction action)
{
    int id;
    iss >> id;

    auto& positions = reg.get_components<component::position>();
    auto& drawables = reg.get_components<component::drawable>();
    auto& velocities = reg.get_components<component::velocity>();

    if (id >= 0 && id < positions.size() && positions[id] && drawables[id] && velocities[id]) {
        switch(action) {
            case GameAction::UP:
                velocities[id].value().vy = -baseSpeed;
                if (animState.currentFrame < MAX_UP_FRAME) {
                    drawables[id].value().sprite.setTextureRect(sf::IntRect(
                        MAX_UP_FRAME * drawables[id].value().sprite.getTextureRect().width,
                        0,
                        drawables[id].value().sprite.getTextureRect().width,
                        drawables[id].value().sprite.getTextureRect().height
                    ));
                }
                break;
            case GameAction::DOWN:
                velocities[id].value().vy = baseSpeed;
                if (animState.currentFrame > MIN_DOWN_FRAME) {
                    drawables[id].value().sprite.setTextureRect(sf::IntRect(
                        MIN_DOWN_FRAME * drawables[id].value().sprite.getTextureRect().width,
                        0,
                        drawables[id].value().sprite.getTextureRect().width,
                        drawables[id].value().sprite.getTextureRect().height
                    ));
                }
                break;
            case GameAction::LEFT:
                velocities[id].value().vx = -baseSpeed;
                break;
            case GameAction::RIGHT:
                velocities[id].value().vx = baseSpeed;
                break;
            case GameAction::STOP_X:
                velocities[id].value().vx = 0;
                break;
            case GameAction::STOP_Y:
                velocities[id].value().vy = 0;
                drawables[id].value().sprite.setTextureRect(sf::IntRect(
                    NEUTRAL_FRAME * drawables[id].value().sprite.getTextureRect().width,
                    0,
                    drawables[id].value().sprite.getTextureRect().width,
                    drawables[id].value().sprite.getTextureRect().height
                ));
                break;
            default:
                break;
        }
    }
}

void Core::handleShootCommands(std::istringstream& iss)
{
    int x, y;
    iss >> x >> y;

    Entity missile = reg.spawn_entity();
    reg.emplace_component<component::position>(missile, component::position{x, y});
    reg.emplace_component<component::velocity>(missile, component::velocity{5, 0});
    sf::Sprite sprite = utils.cat("../ressources/sprites/shoot.png");

    // Set up animation
    sf::IntRect rect(0, 0, sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height);
    sprite.setTextureRect(rect);
    sprite.setScale(2,2);
    reg.emplace_component<component::animation>(missile, component::animation{
        0,          // current frame
        2,          // total frames
        0.1f,       // frame duration in seconds
        sf::Clock() // animation clock
    });
    reg.emplace_component<component::drawable>(missile, component::drawable{sprite});
    reg.emplace_component<component::controllable>(missile, component::controllable{false});
    reg.emplace_component<component::type>(missile, component::type{6});
}

void Core::handleSuperShootCommands(std::istringstream& iss)
{
    int x, y;
    iss >> x >> y;

    Entity missile = reg.spawn_entity();
    reg.emplace_component<component::position>(missile, component::position{x, y});
    reg.emplace_component<component::velocity>(missile, component::velocity{6, 0});
    sf::Sprite sprite = utils.cat("../ressources/sprites/super_shoot.png");

    // Set up animation
    sf::IntRect rect(0, 0, sprite.getGlobalBounds().width / 4, sprite.getGlobalBounds().height);
    sprite.setTextureRect(rect);
    sprite.setScale(4,4);
    reg.emplace_component<component::animation>(missile, component::animation{
        0,          // current frame
        4,          // total frames
        0.08f,      // frame duration in seconds
        sf::Clock() // animation clock
    });
    reg.emplace_component<component::drawable>(missile, component::drawable{sprite});
    reg.emplace_component<component::controllable>(missile, component::controllable{false});
    reg.emplace_component<component::type>(missile, component::type{6});
}

// Add this to your animation system update
void Core::updateAnimations()
{
    auto& drawables = reg.get_components<component::drawable>();
    auto& animations = reg.get_components<component::animation>();
    for (size_t i = 0; i < animations.size(); ++i) {
        if (!animations[i] || !drawables[i]) continue;
        auto& anim = animations[i].value();
        if (anim.clock.getElapsedTime().asSeconds() >= anim.frameDuration) {
            anim.currentFrame = (anim.currentFrame + 1) % anim.totalFrames;
            auto& sprite = drawables[i].value().sprite;
            int frameWidth = sprite.getTextureRect().width;
            sprite.setTextureRect(sf::IntRect(
                anim.currentFrame * frameWidth,
                0,
                frameWidth,
                sprite.getTextureRect().height
            ));
            anim.clock.restart();
        }
    }
}

void Core::handleLaserShootCommands(std::istringstream& iss)
{
    int x, y;
    iss >> x >> y;

    Entity missile = reg.spawn_entity();
    reg.emplace_component<component::position>(missile, component::position{x, y});
    reg.emplace_component<component::velocity>(missile, component::velocity{0, 0});
    sf::Sprite sprite = utils.cat("../ressources/sprites/laser_shoot.png");
    sf::IntRect rect(0, 0, sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height);
    sprite.setTextureRect(rect);
    sprite.setScale(1.5, 6);
    reg.emplace_component<component::animation>(missile, component::animation{
        0,          // current frame
        2,          // total frames
        0.1f,      // frame duration in seconds
        sf::Clock() // animation clock
    });
    reg.emplace_component<component::drawable>(missile, component::drawable{sprite});
    reg.emplace_component<component::controllable>(missile, component::controllable{false});
    reg.emplace_component<component::type>(missile, component::type{6});
}

void Core::handleMobShootCommand(std::istringstream& iss)
{
    int x, y;
    iss >> x >> y;

    Entity missile = reg.spawn_entity();
    reg.emplace_component<component::position>(missile, component::position{x, y});
    reg.emplace_component<component::velocity>(missile, component::velocity{-5, 0});
    sf::Sprite sprite = utils.cat("../ressources/sprites/shoot_mob.png");
    sf::IntRect rect(0, 0, sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height);
    sprite.setTextureRect(rect);
    sprite.setScale(2, 2);
    reg.emplace_component<component::animation>(missile, component::animation{
        0,          // current frame
        2,          // total frames
        0.5f,      // frame duration in seconds
        sf::Clock() // animation clock
    });
    reg.emplace_component<component::drawable>(missile, component::drawable{sprite});
    reg.emplace_component<component::controllable>(missile, component::controllable{false});
    reg.emplace_component<component::type>(missile, component::type{7});
}

void Core::handlePowerUpCommand(std::istringstream& iss)
{
    int type, x, y;
    iss >> type >> x >> y;

    std::cout << "POWERUP SPAWNED -> Type: " << type << "/ x: " << x << "/ y:" << y << std::endl;

    Entity powerup = reg.spawn_entity();
    reg.emplace_component<component::position>(powerup, component::position{x, y});
    sf::Sprite sprite = utils.cat("../ressources/sprites/pow1.png");
    sf::Sprite sprite2 = utils.cat("../ressources/sprites/pow2.png");
    sf::Sprite sprite3 = utils.cat("../ressources/sprites/pow3.png");
    sf::Sprite sprite4 = utils.cat("../ressources/sprites/force.png");
    sf::Sprite sprite5 = utils.cat("../ressources/sprites/meteor.png");
    sprite3.setScale(2.5, 2.5);
    
    if (type == 0) {
        reg.emplace_component<component::drawable>(powerup, component::drawable{sprite});
    } else if (type == 1) {
        reg.emplace_component<component::drawable>(powerup, component::drawable{sprite2});
    } else if (type == 3) {
        reg.emplace_component<component::drawable>(powerup, component::drawable{sprite3});
    } else if (type == 2) {
        sf::IntRect rect(0, 0, sprite4.getGlobalBounds().width / 12, sprite4.getGlobalBounds().height);
        sprite4.setTextureRect(rect);
        sprite4.setScale(3, 3);
        reg.emplace_component<component::drawable>(powerup, component::drawable{sprite4});
        reg.emplace_component<component::animation>(powerup, component::animation{
            0,
            12,
            0.1f,
            sf::Clock()
        });
    }
    else if (type == 50) {
        reg.emplace_component<component::drawable>(powerup, component::drawable{sprite5});
        reg.emplace_component<component::velocity>(powerup, component::velocity{-5, 0});
        sprite5.setScale(4, 4);
    }
    reg.emplace_component<component::type>(powerup, component::type{type});
}

void Core::handleCollisionCommand(std::istringstream& iss)
{
    int id, type;
    iss >> id >> type;
    auto& healths = reg.get_components<component::health>();
    auto& drawables = reg.get_components<component::drawable>();
    auto& invincibles = reg.get_components<component::invincible>();

    if (id >= 0 && id < healths.size() && healths[id]) {
        if (type == 10 && invincibles[id].value().is_invincible == false) {
            handleMobCollision(id, healths, drawables, invincibles);
        }
        else if (type == 0 || type == 1 || type == 2) {
            handlePowerUpCollision(id, type, healths);
        }
        else if (type == 7 && invincibles[id].value().is_invincible == false) {
            handleMobMissileCollision(id, healths, drawables, invincibles);
        }

        updatePlayerHealth(id, healths[id].value().hp);
    }
}

void Core::handleMobCollision(int id, sparse_array<component::health>& healths, 
                            sparse_array<component::drawable>& drawables,
                            sparse_array<component::invincible>& invincibles)
{
    if (!healths[id]) return;
    healths[id].value().hp -= 50;
    invincibles[id].value().is_invincible = true;
    invincibles[id].value().expiration_time = std::chrono::steady_clock::now() + 
                                             std::chrono::seconds(1);
    if (drawables[id]) {
        drawables[id].value().sprite.setColor(sf::Color(255, 255, 255, 128));
    }
}

void Core::handlePowerUpCollision(int id, int type, sparse_array<component::health>& healths)
{
    if (!healths[id]) return;
    switch(type) {
        case 0:
            // empty
            break;
        case 1:
            healths[id].value().hp += 10;
            if (healths[id].value().hp > 100) {
                healths[id].value().hp = 100;
            }
            break;
        case 3:
            if (id == network->getId()) {
                laserActive = true;
                laserClock.restart();
            }
            break;
    }
    powerupSound.play();
}

void Core::handleMobMissileCollision(int id, sparse_array<component::health>& healths,
                                   sparse_array<component::drawable>& drawables,
                                   sparse_array<component::invincible>& invincibles)
{
    if (!healths[id]) return;
    healths[id].value().hp -= 30;
    invincibles[id].value().is_invincible = true;
    invincibles[id].value().expiration_time = std::chrono::steady_clock::now() + 
                                             std::chrono::seconds(1);
    if (drawables[id]) {
        drawables[id].value().sprite.setColor(sf::Color(255, 255, 255, 128));
    }
}

void Core::updatePlayerHealth(int id, int newHealth)
{
    // Find the player in otherPlayers vector and update their health
    for (auto& player : otherPlayers) {
        if (player.id == id) {
            player.hp = newHealth;
            player.hpText.setString("Player " + std::to_string(id) + ": " + std::to_string(newHealth));
            // Change text color based on health level
            if (newHealth <= 25) {
                player.hpText.setFillColor(sf::Color::Red);
            } else if (newHealth <= 50) {
                player.hpText.setFillColor(sf::Color::Yellow);
            } else {
                player.hpText.setFillColor(sf::Color::Blue);
            }
            // Check for player death
            if (newHealth <= 0) {

                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::DEATH) << " " << id;
                network->send(messageStream.str());
            }
            break;
        }
    }
}

void Core::updatePlayerId()
{
    auto& types = reg.get_components<component::type>();

    for (size_t i = 0; i < types.size(); ++i) {
        if (types[i] && types[i].value().type == 696) {
            network->setId(i);
            std::cout << "Mise à jour de l'ID du joueur à " << i << std::endl;
            return;
        }
    }
    network->setId(-1);
    std::cout << "Aucune entité contrôlable trouvée, ID du joueur défini à -1" << std::endl;
}

void Core::handleWinCommand(std::istringstream& iss)
{
    gui_gamewin();
}

void Core::handleStartCommand(std::istringstream& iss)
{
    std::cout << "START THE GAME COTE CLIENT";
    gui_game();
}

void Core::handleDeathCommand(std::istringstream& iss)
{
    int id;
    iss >> id;

    auto& healths = reg.get_components<component::health>();
    if (id == network->getId()) {
        std::cout << "Mort du joueur " << id << std::endl;
        healths[id].value().hp = 0;
        updatePlayerHealth(id, healths[id].value().hp);
        isDead = true;
    }
    reg.kill_entity(Entity(id));
    updatePlayerId();
}

void Core::handleDisconnectCommand(std::istringstream& iss)
{
    int id;
    iss >> id;
    std::cout << "Deconnexion du joueur " << id << std::endl;

    // Trouver et supprimer le joueur avec l'ID spécifique
    auto it = std::find_if(otherPlayers.begin(), otherPlayers.end(),
        [id](const PlayerInfo& player) {
            return player.id == id;
        });

    if (it != otherPlayers.end()) {
        otherPlayers.erase(it);
    }
    reg.kill_entity(Entity(id));
    nb_player--;
    updatePlayerId();
}