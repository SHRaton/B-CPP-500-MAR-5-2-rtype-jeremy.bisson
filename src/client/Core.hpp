#pragma once

#include "../Entity.hpp"
#include "../Registry.hpp"
#include "../SparseArray.hpp"
#include "../Components.hpp"
#include "../Systems.hpp"
#include <cstddef>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Sprite.hpp"
#include "UDPNetworkClient.hpp"

class Core {
    public :
        Core();
        ~Core();

        //*************** Graphic ***************//
        void loadAssets();
        void gui(int argc, char**argv);
        // Menu D'acceuil
        void gui_menu();
        // Menu de Login NAME IP PORT
        void gui_login();
        void handleKeyboard(sf::Event::KeyEvent key);
        void handleMouseClick(sf::Vector2i mousePosition);
        // Game
        void gui_game();
        //*************** Graphic ***************//

        //*************** ECS ***************//
        registry reg;
        //*************** ECS ***************//

    private:
        Utils utils;

        // Network
        void initialize_network(const std::string& server_ip, uint16_t port);
        std::unique_ptr<UDPNetworkClient> network;

        // Window
        sf::RenderWindow window;

        // Strings
        std::string str_name;
        std::string str_ip;
        std::string str_port;
        std::string str_failed;
        std::string buffer;

        // Int
        int select_button;
        int failed_connection;

        // Textes
        sf::Font font;
        sf::Text text_name;
        sf::Text text_ip;
        sf::Text text_port;
        sf::Text text_failed;

        std::map<std::string, Sprite> sprites_menu;
        std::vector<std::string> drawOrder_menu;

        std::map<std::string, Sprite> sprites_login;
        std::vector<std::string> drawOrder_login;

};
