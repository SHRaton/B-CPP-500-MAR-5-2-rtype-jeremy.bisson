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
#include <sstream>
#include "../server/ServerNetwork.hpp"

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
        void login();
        void login_auto(std::string ip, std::string port);
        void handleKeyboard(sf::Event::KeyEvent key);
        void handleMouseClick(sf::Vector2i mousePosition);
        // Game
        void gui_game();
        void handleCommands(std::string command);
        void handleMoove();
        //*************** Graphic ***************//

        //*************** ECS ***************//
        registry reg;
        Systems sys;
        //*************** ECS ***************//

    private:
        Utils utils;

        // Network
        void initialize_network(const std::string& server_ip, uint16_t port);
        std::unique_ptr<UDPNetworkClient> network;

        // Fps
        int fps;

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
        sf::Text fpsText; // Texte pour afficher les FPS
        sf::Text latencyText; // Texte pour afficher la latence

        // Clocks
        sf::Clock fpsClock; // Horloge pour mesurer le temps entre les frames
        sf::Clock latencyClock; // Horloge pour simuler la mesure de la latence
        float baseSpeed; // Vitesse de base en pixels par seconde
        sf::Clock deltaClock;         // Horloge pour mesurer le delta time

        // Sprites
        std::unordered_map<sf::Keyboard::Key, bool> keysPressed;
        sf::Sprite vaisseau;

        std::map<std::string, Sprite> sprites_menu;
        std::vector<std::string> drawOrder_menu;

        std::map<std::string, Sprite> sprites_login;
        std::vector<std::string> drawOrder_login;

        std::map<std::string, Sprite> sprites_game;
        std::vector<std::string> drawOrder_game;

        std::map<int, sf::Sprite> other_players;


        void control_system();
        void load_spaceship();
        sf::Texture texture_vaisseau0;
        sf::Texture texture_vaisseau1;
        sf::Texture texture_vaisseau2;
        sf::Texture texture_vaisseau3;
        sf::Clock clock_mess;
        std::string encode_action(GameAction action);
        // Constants
        static constexpr float FRAME_DURATION = 0.05f;
        static constexpr int MAX_FRAMES = 4;
        // Animation state
        struct AnimationState {
            int currentFrame = 0;
            float animationTimer = 0.0f;
        };

        // Input state
        struct InputState {
            bool upSent = false;
            bool downSent = false;
            bool leftSent = false;
            bool rightSent = false;
        };

        void handle_vertical_movement(float deltaSeconds, std::optional<component::velocity>& vel, std::optional<component::drawable>& drawable);
        void handle_horizontal_movement(float deltaSeconds, std::optional<component::velocity>& vel);
        void update_animation(float deltaSeconds, std::optional<component::drawable>& drawable);
        void handle_idle_animation(float deltaSeconds, std::optional<component::drawable>& drawable);
        void update_sprite_frame(sf::Sprite& sprite);
        void send_input_if_needed(GameAction action, bool& sentFlag);
        void handle_vertical_stop(std::optional<component::velocity>& vel);
        void handle_horizontal_stop(std::optional<component::velocity>& vel);

        AnimationState animState;
        InputState inputState;

        Entity player;
        std::unordered_map<int, Entity> player_id_map;
        int nb_player;





        //TEST
        void setup_position_timer(boost::asio::steady_timer& position_timer);
        std::unique_ptr<boost::asio::steady_timer> position_timer_;
        boost::asio::io_context io_context_;
        std::thread io_thread_;
};
