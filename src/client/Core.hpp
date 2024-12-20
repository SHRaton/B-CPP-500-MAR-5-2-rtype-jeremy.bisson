#pragma once

#include "../Entity.hpp"
#include "../Registry.hpp"
#include "../SparseArray.hpp"
#include "../Components.hpp"
#include "../Systems.hpp"
#include <cstddef>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include "Sprite.hpp"
#include "UDPNetworkClient.hpp"
#include <sstream>
#include "../server/ServerNetwork.hpp"

enum DaltonismType {
    PROTANOPIA,    // Rouge-vert (faiblesse des cônes rouges)
    DEUTERANOPIA,  // Rouge-vert (faiblesse des cônes verts)
    TRITANOPIA,    // Bleu-jaune (rare)
    NONE
};

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
        bool loadDaltonismShader(sf::Shader& shader, DaltonismType type);
        // Option
        void gui_option();
        //*************** Graphic ***************//

        //*************** ECS ***************//
        registry reg;
        Systems sys;
        //*************** ECS ***************//


        void gui_gameover();
        void gui_gamewin();

    private:
        Utils utils;

        // Network
        void initialize_network(const std::string& server_ip, uint16_t port);
        std::unique_ptr<UDPNetworkClient> network;

        // Fps
        int fps;

        // Window
        sf::RenderWindow window;
        sf::RenderWindow registryWindow;

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
        sf::Text hp;

        // Clocks
        sf::Clock fpsClock; // Horloge pour mesurer le temps entre les frames
        sf::Clock latencyClock; // Horloge pour simuler la mesure de la latence
        float baseSpeed; // Vitesse de base en pixels par seconde
        sf::Clock deltaClock;         // Horloge pour mesurer le delta time

        // Sprites
        std::unordered_map<sf::Keyboard::Key, bool> keysPressed;
        sf::Sprite vaisseau;
        sf::RectangleShape shootBar;

        std::map<std::string, Sprite> sprites_menu;
        std::vector<std::string> drawOrder_menu;

        std::map<std::string, Sprite> sprites_option;
        std::vector<std::string> drawOrder_option;
        sf::RectangleShape soundBar;
        sf::RectangleShape soundBarHitBox;
        sf::Text text_option;
        sf::Text text_graphics;
        sf::Text text_sound;
        sf::Text text_sound_general;
        float soundVolume;

        // Texture to concat all draw and apply Shader on texture
        sf::RenderTexture renderTexture;

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
            bool shootSent = false;
        };

        float shootCooldown;

        void handle_vertical_movement(float deltaSeconds, std::optional<component::velocity>& vel,
        std::optional<component::drawable>& drawable, std::optional<component::position>& pos);
        void handle_horizontal_movement(float deltaSeconds, std::optional<component::velocity>& vel,
        std::optional<component::drawable>& drawable, std::optional<component::position>& pos);
        void update_animation(float deltaSeconds, std::optional<component::drawable>& drawable);
        void handle_idle_animation(float deltaSeconds, std::optional<component::drawable>& drawable);
        void update_sprite_frame(sf::Sprite& sprite);
        void send_input_if_needed(GameAction action, bool& sentFlag);
        void handle_vertical_stop(std::optional<component::velocity>& vel);
        void handle_horizontal_stop(std::optional<component::velocity>& vel);
        void handle_shoot(float deltaSeconds, std::optional<component::position>& pos);

        void handleServerCommands(const std::string& command);
        void handleMoveCommand(std::istringstream& iss);
        void handleMobSpawnCommand(std::istringstream& iss);
        void handleConnectCommand(std::istringstream& iss);
        void handleMovementCommands(std::istringstream& iss, GameAction action);
        void handleShootCommands(std::istringstream& iss);
        void handleMobShootCommand(std::istringstream& iss);
        void handlePowerUpCommand(std::istringstream& iss);
        void handleCollisionCommand(std::istringstream& iss);
        void handleDeathCommand(std::istringstream& iss);
        void updatePlayerHealth(int id, int newHealth);
        void handlePowerUpCollision(int id, int type, sparse_array<component::health>& healths);
        void handleMobMissileCollision(int id, sparse_array<component::health>& healths,
                                   sparse_array<component::drawable>& drawables,
                                   sparse_array<component::invincible>& invincibles);
        void handleMobCollision(int id, sparse_array<component::health>& healths, 
                            sparse_array<component::drawable>& drawables,
                            sparse_array<component::invincible>& invincibles);

        void update_hud();
        void displayRegistryInfo();
        sf::Text registryInfo;

        AnimationState animState;
        InputState inputState;

        Entity player;
        std::unordered_map<int, Entity> player_id_map;
        
        struct PlayerInfo {
            int id;
            int hp;
            sf::Text hpText;
        };
        std::vector<PlayerInfo> otherPlayers;
        int nb_player;

        //SOUNDS

        sf::Music menuMusic;
        sf::Music Game1Music;

        sf::Music gameoverMusic;
        sf::Music winMusic;

        sf::SoundBuffer buttonBuffer_click;
        sf::Sound buttonSound_click;

        sf::SoundBuffer powerupBuffer;
        sf::Sound powerupSound;

        sf::SoundBuffer shotBuffer;
        sf::Sound shotSound;


        //TEST
        void setup_position_timer(boost::asio::steady_timer& position_timer);
        void checkInvincibility();
        std::unique_ptr<boost::asio::steady_timer> invulnerability_timer_;
        std::unique_ptr<boost::asio::steady_timer> position_timer_;
        boost::asio::io_context io_context_;
        std::thread io_thread_;

        //ANIMATION VAISSEAU
        void animate_up(float deltaSeconds, std::optional<component::drawable>& drawable);
        void animate_down(float deltaSeconds, std::optional<component::drawable>& drawable);
        void animate_to_neutral(float deltaSeconds, std::optional<component::drawable>& drawable);
        static constexpr float FRAME_DURATION = 0.05f;
        const int NEUTRAL_FRAME = 2;
        const int MAX_UP_FRAME = 4;
        const int MIN_DOWN_FRAME = 0;

        DaltonismType daltonismType;
        sf::Shader daltonismShader;
        sf::RenderStates states;

        //GAMEOVER SCREEN
        sf::Sprite gameOverSprite;
        sf::Texture gameOverTexture;

        //WIN SCREEN
        sf::Sprite gameWinSprite;
        sf::Texture gameWinTexture;

};
