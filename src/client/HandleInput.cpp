/*
** EPITECH PROJECT, 2024
** B-CPP-500-MAR-5-2-rtype-jeremy.bisson
** File description:
** HandleInput
*/

#include "Core.hpp"
#include "../Systems.hpp"
#include "../server/ServerNetwork.hpp"

void Core::handle_horizontal_movement(float deltaSeconds, std::optional<component::velocity>& vel,
                                    std::optional<component::drawable>& drawable, std::optional<component::position>& pos)
{
    if (!vel) return;
    
    bool leftPressed = keysPressed[keyBindings["Left"].getKey()];
    bool rightPressed = keysPressed[keyBindings["Right"].getKey()];
    
    // Determine the effective direction based on both keys
    if (leftPressed && rightPressed) {
        // If both keys are pressed, prioritize the last pressed key
        // Alternative: could stop movement instead
        handle_horizontal_stop(vel);
        inputState.leftSent = false;
        inputState.rightSent = false;
        return;
    }
    
    // Handle movement based on single key press
    if (leftPressed) {
        if (pos.value().x < 0) {
            handle_horizontal_stop(vel);
        } else {
            vel->vx = -baseSpeed;
            send_input_if_needed(GameAction::LEFT, inputState.leftSent);
        }
    } else {
        inputState.leftSent = false;
    }

    if (rightPressed) {
        if (pos.value().x > 1810) {
            handle_horizontal_stop(vel);
        } else {
            vel->vx = baseSpeed;
            send_input_if_needed(GameAction::RIGHT, inputState.rightSent);
        }
    } else {
        inputState.rightSent = false;
    }

    // If no movement keys are pressed, stop
    if (!leftPressed && !rightPressed) {
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
    shootCooldown = std::min(shootCooldown, 0.3f);
    shootBar.setSize(sf::Vector2f(200 * shootCooldown, 5));
    shootBar.setPosition(pos.value().x, pos.value().y - 20);

    laserPowerUpLogo.setScale(0.1, 0.1);
    laserPowerUpLogo.setPosition(pos.value().x + 70, pos.value().y - 40);

    forcePowerUpLogo.setScale(3, 3);
    forcePowerUpLogo.setPosition(pos.value().x + 100, pos.value().y);

    if (keysPressed[keyBindings["Shoot"].getKey()] && shootCooldown >= 0.3) {
        send_input_if_needed(GameAction::SHOOT, inputState.shootSent);
        if (!laserActive) {
            shotSound.play();
            shootCooldown = 0.0f;
        } else if (laserActive) {
            laserSound.play();
        }
    } else {
        inputState.shootSent = false;
    }

    superShootCooldown += deltaSeconds;
    superShootCooldown = std::min(superShootCooldown, 15.0f);
    superShootBar.setSize(sf::Vector2f(4 * superShootCooldown, 5));
    superShootBar.setPosition(pos.value().x, pos.value().y - 30);

    if (keysPressed[keyBindings["SuperShoot"].getKey()] && superShootCooldown >= 15) {
        send_input_if_needed(GameAction::SUPER_SHOOT, inputState.superShootSent);
        superShotSound.play();
        superShootCooldown = 0.0f;
    } else {
        inputState.superShootSent = false;
    }
}

void Core::handle_start(std::optional<component::position>& pos)
{
    if (!pos) return;

    if (keysPressed[sf::Keyboard::Space]) {
        send_input_if_needed(GameAction::START, inputState.startSent);
    } else {
        inputState.startSent = false;
    }
}

void Core::handle_hitbox()
{
    if (keysPressed[keyBindings["Hitbox"].getKey()]) {
        if (!inputState.hitboxSent) {
           inputState.hitboxSent = true;
            if (isHitbox == false) {
                isHitbox = true;
            } else if (isHitbox == true) {
                isHitbox = false;
            }
        }
    } else {
        inputState.hitboxSent = false;
    }
}

// Modification de Core::handle_horizontal_movement et handle_vertical_movement
void Core::handle_movement_update(float deltaSeconds, std::optional<component::velocity>& vel,
                                std::optional<component::drawable>& drawable, 
                                std::optional<component::position>& pos)
{
    if (!vel || !pos || !drawable) return;

    bool leftPressed = keysPressed[keyBindings["Left"].getKey()];
    bool rightPressed = keysPressed[keyBindings["Right"].getKey()];
    bool upPressed = keysPressed[keyBindings["Up"].getKey()];
    bool downPressed = keysPressed[keyBindings["Down"].getKey()];

    float newVx = vel->vx;
    float newVy = vel->vy;
    bool stateChanged = false;

    // Gestion du mouvement horizontal avec vérification des limites
    if (leftPressed && !rightPressed) {
        if (pos.value().x < 0) {
            if (newVx != 0) {
                newVx = 0;
                stateChanged = true;
                send_input_if_needed(GameAction::STOP_X, inputState.horizontalStopSent);
            }
        } else {
            if (newVx != -baseSpeed) {
                newVx = -baseSpeed;
                stateChanged = true;
                send_input_if_needed(GameAction::LEFT, inputState.leftSent);
            }
        }
    } else if (rightPressed && !leftPressed) {
        if (pos.value().x > 1810) {
            if (newVx != 0) {
                newVx = 0;
                stateChanged = true;
                send_input_if_needed(GameAction::STOP_X, inputState.horizontalStopSent);
            }
        } else {
            if (newVx != baseSpeed) {
                newVx = baseSpeed;
                stateChanged = true;
                send_input_if_needed(GameAction::RIGHT, inputState.rightSent);
            }
        }
    } else if (!leftPressed && !rightPressed && newVx != 0) {
        newVx = 0;
        stateChanged = true;
        send_input_if_needed(GameAction::STOP_X, inputState.horizontalStopSent);
    }

    // Gestion du mouvement vertical avec vérification des limites et animations
    if (upPressed && !downPressed) {
        if (pos.value().y < 0) {
            if (newVy != 0) {
                newVy = 0;
                stateChanged = true;
                send_input_if_needed(GameAction::STOP_Y, inputState.verticalStopSent);
            }
            animate_to_neutral(deltaSeconds, drawable);
        } else {
            if (newVy != -baseSpeed) {
                newVy = -baseSpeed;
                stateChanged = true;
                send_input_if_needed(GameAction::UP, inputState.upSent);
            }
            animate_up(deltaSeconds, drawable);
        }
    } else if (downPressed && !upPressed) {
        if (pos.value().y > 970) {  // Utilisation de 970 comme dans le code original
            if (newVy != 0) {
                newVy = 0;
                stateChanged = true;
                send_input_if_needed(GameAction::STOP_Y, inputState.verticalStopSent);
            }
            animate_to_neutral(deltaSeconds, drawable);
        } else {
            if (newVy != baseSpeed) {
                newVy = baseSpeed;
                stateChanged = true;
                send_input_if_needed(GameAction::DOWN, inputState.downSent);
            }
            animate_down(deltaSeconds, drawable);
        }
    } else if (!upPressed && !downPressed) {
        if (newVy != 0) {
            newVy = 0;
            stateChanged = true;
            send_input_if_needed(GameAction::STOP_Y, inputState.verticalStopSent);
        }
        animate_to_neutral(deltaSeconds, drawable);
    }

    // Mise à jour de la vélocité seulement si nécessaire
    if (stateChanged) {
        vel->vx = newVx;
        vel->vy = newVy;
    }

    // Réinitialisation des flags d'envoi
    if (!leftPressed) inputState.leftSent = false;
    if (!rightPressed) inputState.rightSent = false;
    if (!upPressed) inputState.upSent = false;
    if (!downPressed) inputState.downSent = false;
    if (newVx == 0) inputState.horizontalStopSent = false;
    if (newVy == 0) inputState.verticalStopSent = false;
}

void Core::control_system()
{
    if (isReplay){
        return;
    }
    float deltaSeconds = deltaClock.restart().asSeconds();
    auto const &controllables = reg.get_components<component::controllable>();
    auto &velocities = reg.get_components<component::velocity>();
    auto &positions = reg.get_components<component::position>();
    auto &drawables = reg.get_components<component::drawable>();
    auto &healths = reg.get_components<component::health>();
    auto &types = reg.get_components<component::type>();
    for (size_t i = 0; i < controllables.size(); ++i) {
        auto const &controllable = controllables[i];
        auto &vel = velocities[i];
        auto &pos = positions[i];
        auto &drawable = drawables[i];
        auto &health = healths[i];
        auto &type = types[i];
        if (isDead == true && type && type.value().type == 696) {
            break;
        }
        if (controllable && vel && drawable && controllable.value().is_controllable && pos) {
            handle_movement_update(deltaSeconds, vel, drawable, pos);
            handle_shoot(deltaSeconds, pos);
            handle_hitbox();
        }
    }
}
