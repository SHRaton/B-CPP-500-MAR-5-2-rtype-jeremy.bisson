/**
 * @brief Classe utilitaire fournissant des fonctions communes
 * @class Utils
 * 
 * @details Cette classe fournit des fonctions utilitaires pour la gestion des sprites,
 * l'affichage dans le terminal et la journalisation.
 * 
 * @startuml{Utils_class.png}
 * class Utils {
 * + Utils()
 * + ~Utils()
 * + cat(path: string): sf::Sprite
 * + setOriginToMiddle(sprite: sf::Sprite&)
 * + printTerminal(color: string, mess: string)
 * + printLog(mess: string)
 * }
 * 
 * namespace Color {
 * + {static} RESET: string
 * + {static} RED: string
 * + {static} GREEN: string
 * + {static} YELLOW: string
 * + {static} BLUE: string
 * }
 * 
 * Utils ..> "uses" Color
 * @enduml
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>

/**
 * @brief Namespace contenant les codes couleur pour l'affichage terminal
 * @namespace Color
 */
namespace Color {
    const std::string RESET   = "\033[0m";  ///< Réinitialise la couleur
    const std::string RED     = "\033[31m"; ///< Couleur rouge
    const std::string GREEN   = "\033[32m"; ///< Couleur verte
    const std::string YELLOW  = "\033[33m"; ///< Couleur jaune
    const std::string BLUE    = "\033[34m"; ///< Couleur bleue
}

class Utils {
    public:
        Utils() = default;
        ~Utils() = default;

        /**
         * @brief Charge et retourne un sprite à partir d'un chemin de fichier
         * @param path Chemin vers le fichier image
         * @return sf::Sprite Sprite créé
         */
        sf::Sprite cat(const std::string& path);

        /**
         * @brief Centre l'origine d'un sprite
         * @param sprite Sprite à modifier
         */
        void setOriginToMiddle(sf::Sprite& sprite);

        /**
         * @brief Affiche un message coloré dans le terminal
         * @param color Couleur du message
         * @param mess Message à afficher
         */
        void printTerminal(std::string color, std::string mess);

        /**
         * @brief Enregistre un message dans les logs
         * @param mess Message à enregistrer
         */
        void printLog(std::string mess);

    private:
};