/*
** EPITECH PROJECT, 2024
** B-CPP-500-MAR-5-2-rtype-jeremy.bisson
** File description:
** *
*/

#include "Core.hpp"
#include "../Systems.hpp"
#include "../server/ServerNetwork.hpp"

//===================================ANIMATION=================================


void Core::handle_vertical_movement(float deltaSeconds, std::optional<component::velocity>& vel,
                                    std::optional<component::drawable>& drawable, std::optional<component::position>& pos)
{
    if (!vel || !drawable) return;

    // Handle upward movement
    if (keysPressed[sf::Keyboard::Up]) {
        if (pos.value().y < 0) {
            handle_vertical_stop(vel);
        } else {
            vel->vy = -baseSpeed;
            animate_up(deltaSeconds, drawable);
            send_input_if_needed(GameAction::UP, inputState.upSent);
        }
    }
    // Handle downward movement
    else if (keysPressed[sf::Keyboard::Down]) {
        if (pos.value().y > 970) {
            handle_vertical_stop(vel);
        } else {
            vel->vy = baseSpeed;
            animate_down(deltaSeconds, drawable);
            send_input_if_needed(GameAction::DOWN, inputState.downSent);
        }
    }
    // Return to neutral position when no vertical movement
    else {
        inputState.upSent = false;
        inputState.downSent = false;
        animate_to_neutral(deltaSeconds, drawable);
        handle_vertical_stop(vel);
    }
}

void Core::animate_up(float deltaSeconds, std::optional<component::drawable>& drawable)
{
    if (!drawable) return;
    animState.animationTimer += deltaSeconds;
    if (animState.animationTimer >= FRAME_DURATION) {
        animState.animationTimer = 0;
        if (animState.currentFrame < MAX_UP_FRAME) {
            animState.currentFrame++;
            update_sprite_frame(drawable->sprite);
        }
    }
}

void Core::animate_down(float deltaSeconds, std::optional<component::drawable>& drawable)
{
    if (!drawable) return;
    animState.animationTimer += deltaSeconds;
    if (animState.animationTimer >= FRAME_DURATION) {
        animState.animationTimer = 0;
        if (animState.currentFrame > MIN_DOWN_FRAME) {
            animState.currentFrame--;
            update_sprite_frame(drawable->sprite);
        }
    }
}

void Core::animate_to_neutral(float deltaSeconds, std::optional<component::drawable>& drawable)
{
    if (!drawable) return;
    animState.animationTimer += deltaSeconds;
    if (animState.animationTimer >= FRAME_DURATION) {
        animState.animationTimer = 0;
        if (animState.currentFrame > NEUTRAL_FRAME) {
            animState.currentFrame--;
        } else if (animState.currentFrame < NEUTRAL_FRAME) {
            animState.currentFrame++;
        }
        update_sprite_frame(drawable->sprite);
    }
}

void Core::update_sprite_frame(sf::Sprite& sprite)
{
    sprite.setTextureRect(sf::IntRect(
        animState.currentFrame * sprite.getTextureRect().width,
        0,
        sprite.getTextureRect().width,
        sprite.getTextureRect().height
    ));
}

void Core::handle_idle_animation(float deltaSeconds, std::optional<component::drawable>& drawable)
{
    if (!drawable) return;
    if (animState.currentFrame > 0) {
        animState.animationTimer += deltaSeconds;
        if (animState.animationTimer >= FRAME_DURATION) {
            animState.animationTimer = 0;
            animState.currentFrame--;
            update_sprite_frame(drawable->sprite);
        }
    }
}

//===================================ANIMATION=================================