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

void Core::handle_vertical_movement(float deltaSeconds, std::optional<component::velocity>& vel,
                                    std::optional<component::drawable>& drawable, std::optional<component::position>& pos)
{
    if (!vel || !drawable) return;

    // Handle upward movement
    if (keysPressed[keyBindings["Up"].getKey()]) {
        if (pos.value().y < 0) {
            handle_vertical_stop(vel);
        } else {
            vel->vy = -baseSpeed;
            animate_up(deltaSeconds, drawable);
            send_input_if_needed(GameAction::UP, inputState.upSent);
        }
    }
    // Handle downward movement
    else if (keysPressed[keyBindings["Down"].getKey()]) {
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

void Core::startExplosionAt(float x, float y)
{
    explosionSound.play();
    ExplosionAnimation explosion;
    explosion.sprite.setTexture(explosionTexture);
    explosion.sprite.setTextureRect(sf::IntRect(0, 0, 
        ExplosionAnimation::FRAME_WIDTH, 
        ExplosionAnimation::FRAME_HEIGHT));
    explosion.sprite.setPosition(x, y);
    explosion.sprite.setScale(3, 3);
    explosion.isActive = true;
    explosion.x = x;
    explosion.y = y;
    
    // Centre l'origine du sprite
    explosion.sprite.setOrigin(
        ExplosionAnimation::FRAME_WIDTH / 2.0f,
        ExplosionAnimation::FRAME_HEIGHT / 2.0f
    );
    
    activeExplosions.push_back(explosion);
}

void Core::updateExplosions(float deltaSeconds) {
    for (auto& explosion : activeExplosions) {
        if (!explosion.isActive) continue;
        
        explosion.animationTimer += deltaSeconds;
        if (explosion.animationTimer >= ExplosionAnimation::FRAME_DURATION) {
            explosion.animationTimer = 0;
            explosion.currentFrame++;
            
            if (explosion.currentFrame >= ExplosionAnimation::MAX_FRAMES) {
                explosion.isActive = false;
            } else {
                explosion.sprite.setTextureRect(sf::IntRect(
                    explosion.currentFrame * ExplosionAnimation::FRAME_WIDTH,
                    0,
                    ExplosionAnimation::FRAME_WIDTH,
                    ExplosionAnimation::FRAME_HEIGHT
                ));
            }
        }
    }
    
    // Nettoie les explosions terminées
    activeExplosions.erase(
        std::remove_if(activeExplosions.begin(), activeExplosions.end(),
            [](const ExplosionAnimation& explosion) { return !explosion.isActive; }),
        activeExplosions.end()
    );
}

//===================================ANIMATION=================================