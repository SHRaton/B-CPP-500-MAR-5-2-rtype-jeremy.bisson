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
    shootCooldown = std::min(shootCooldown, 0.3f);
    shootBar.setSize(sf::Vector2f(200 * shootCooldown, 5));
    shootBar.setPosition(pos.value().x, pos.value().y - 20);

    laserPowerUpLogo.setScale(0.1, 0.1);
    laserPowerUpLogo.setPosition(pos.value().x + 70, pos.value().y - 40);

    if (keysPressed[sf::Keyboard::A] && shootCooldown >= 0.3) {
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

    if (keysPressed[sf::Keyboard::E] && superShootCooldown >= 15) {
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

void Core::control_system()
{
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
            handle_vertical_movement(deltaSeconds, vel, drawable, pos);
            handle_horizontal_movement(deltaSeconds, vel, drawable, pos);
            handle_shoot(deltaSeconds, pos);
        }
    }
}
