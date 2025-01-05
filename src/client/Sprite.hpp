#pragma once
#include <cstddef>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <unordered_map>
#include "Utils.hpp"

/**
 * @brief Classe de gestion des sprites avec fonctionnalités avancées
 * @class Sprite
 * 
 * @details Cette classe encapsule un sprite SFML en ajoutant des fonctionnalités
 * comme le scrolling, l'animation au survol et le positionnement intelligent.
 * 
 * @startuml{Sprite_class.png}
 * class Sprite {
 * - sprite: sf::Sprite
 * - hoverSprite: sf::Sprite
 * - isScrolling: bool
 * - clock: sf::Clock
 * - scrollSpeed: float
 * - updateInterval: int
 * - isHoverable: bool
 * - isHovered: bool
 * - isGameBackground: bool
 * + Sprite()
 * + update()
 * + update_xy(x: int, y: int)
 * + setHovered(hover: bool)
 * + isMouseOver(window: sf::RenderWindow): bool
 * }
 * 
 * Sprite *-- "1" sf::Sprite
 * Sprite *-- "1" sf::Clock
 * @enduml
 */

class Sprite {
    public:
    
        Sprite() = default;
         /**
         * @brief Constructeur avec paramètres
         * @param texturePath Chemin vers la texture principale
         * @param scrolling Activation du défilement
         * @param speed Vitesse de défilement
         * @param interval Intervalle de mise à jour
         * @param hoverTexturePath Chemin vers la texture de survol
         */
        Sprite(const std::string& texturePath, bool scrolling = false, float speed = 1.0f,
                int interval = 50, const std::string& hoverTexturePath = "");
        /**
         * @brief Définit le sprite comme arrière-plan du jeu
         */
        void setAsGameBackground() { isGameBackground = true; };
        /**
         * @brief Met à jour l'état du sprite (animation, scrolling)
         */
        void update();
        /**
         * @brief Met à jour la position du sprite
         * @param x Position en x
         * @param y Position en y
         */
        void update_xy(int x, int y);
        /**
         * @brief Définit l'état de survol du sprite
         * @param hover État de survol
         */
        void setHovered(bool hover);
        /**
         * @brief Vérifie si la souris survole le sprite
         * @param window Fenêtre SFML
         * @return true si la souris survole le sprite
         */
        bool isMouseOver(const sf::RenderWindow& window);
        /**
         * @brief Définit la position du sprite
         * @param position Position du sprite
         */
        void setScale(sf::Vector2f scale);
        /**
         * @brief Définit l'origine du sprite au milieu
         */
        void setOriginToMiddle();
        /**
         * @brief Définit la position du sprite
         * @param position Position du sprite
         */
        void setPosition(sf::Vector2f position);
        /**
         * @brief Retourne le sprite
         * @return sf::Sprite Sprite SFML
         */
        sf::Sprite getSprite();
        /**
         * @brief Retourne le sprite de survol
         * @return sf::Sprite Sprite SFML
         */

    private:
        sf::Sprite sprite;           ///< Sprite principal
        sf::Sprite hoverSprite;      ///< Sprite affiché au survol
        bool isScrolling = false;    ///< Indique si le sprite défile
        sf::Clock clock;             ///< Horloge pour les animations
        float scrollSpeed = 0;       ///< Vitesse de défilement
        int updateInterval = 0;      ///< Intervalle entre les mises à jour
        bool isHoverable = false;    ///< Indique si le survol est activé
        bool isHovered = false;      ///< État de survol actuel
        bool isGameBackground = false; ///< Indique si c'est l'arrière-plan
};
