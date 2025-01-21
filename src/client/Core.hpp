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
#include "KeyBinding.hpp"
#include "LevelEditor.hpp"
#include "CurrentMap.hpp"
#include "EntityPosition.hpp"

/**
 * @brief Classe principale du client de jeu
 * @class Core
 * 
 * @details Cette classe gère l'ensemble du client de jeu, incluant l'interface graphique,
 * le réseau, le son, et l'état du jeu. Elle coordonne tous les aspects du jeu côté client.
 * 
 * @startuml{Core_class.png}
 * class Core {
 * + reg: registry
 * + sys: Systems
 * - window: sf::RenderWindow
 * - network: unique_ptr<UDPNetworkClient>
 * - sprites: map<string, Sprite>
 * + Core()
 * + loadAssets()
 * + gui(argc: int, argv: char**)
 * + gui_menu()
 * + gui_game()
 * }
 * 
 * enum DaltonismType {
 * PROTANOPIA
 * DEUTERANOPIA
 * TRITANOPIA
 * NONE
 * }
 * 
 * struct AnimationState {
 * + currentFrame: int
 * + animationTimer: float
 * }
 * 
 * struct InputState {
 * + upSent: bool
 * + downSent: bool
 * + leftSent: bool
 * + rightSent: bool
 * }
 * 
 * Core *-- DaltonismType
 * Core *-- AnimationState
 * Core *-- InputState
 * Core *-- "1" registry
 * Core *-- "1" Systems
 * Core *-- "*" Sprite
 * Core *-- "1" UDPNetworkClient
 * @enduml
 * 
 * @see registry
 * @see Systems
 * @see UDPNetworkClient
 * @see Sprite
 */

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
        void loadAssetsGame();
        void gui(int argc, char**argv);
        // Menu D'acceuil
        void gui_menu();
        // Menu de Login NAME IP PORT
        void gui_login();
        void login();
        void login_auto(std::string ip, std::string port);
        void handleKeyboard(sf::Event::KeyEvent key);
        void handleMouseClick(sf::Vector2i mousePosition);
        // Lobby
        void gui_lobby();
        // Game
        void gui_game();
        void display_all();
        void draw_hitbox();
        bool isHitbox;
        bool loadDaltonismShader(sf::Shader& shader, DaltonismType type);
        bool loadBlackAndWhiteShader();
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
        CurrentMap currentMap;
        bool isScrollingBackground;
        sf::Clock clock_scrolling;
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
        int globalScore = 0;

        // Textes
        sf::Font font;
        sf::Text text_name;
        sf::Text text_ip;
        sf::Text text_port;
        sf::Text text_failed;
        sf::Text fpsText; // Texte pour afficher les FPS
        sf::Text latencyText; // Texte pour afficher la latence
        sf::Text hp;
        sf::Text globalScore_text;

        // Clocks
        sf::Clock fpsClock; // Horloge pour mesurer le temps entre les frames
        sf::Clock latencyClock; // Horloge pour simuler la mesure de la latence
        float baseSpeed; // Vitesse de base en pixels par seconde
        sf::Clock deltaClock;         // Horloge pour mesurer le delta time

        // Sprites
        std::unordered_map<sf::Keyboard::Key, bool> keysPressed;
        sf::Sprite vaisseau;
        sf::RectangleShape shootBar;
        sf::RectangleShape superShootBar;

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

        sf::Vector2i mousePosition;
        sf::Vector2f worldMousePosition;
        sf::Sprite background_lobby;
        sf::Sprite background_lobby2;
        sf::Sprite left_arrow_lobby;
        sf::Sprite right_arrow_lobby;
        int currentSkin_id = 0;
        int nb_skins = 7;
        void updateCurrentSkin();
        sf::Sprite currentSkin;
        sf::Sprite ready;
        sf::Sprite start;
        sf::Sprite replay;
        sf::Sprite level_editor;

        // Texture to concat all draw and apply Shader on texture
        sf::RenderTexture renderTexture;

        std::map<std::string, Sprite> sprites_login;
        std::vector<std::string> drawOrder_login;

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
            bool superShootSent = false;
            bool startSent = false;
            bool hitboxSent = false;
            bool levelEditorSent = false;
            bool changeSkinSent = false;
        };

        float shootCooldown;
        float superShootCooldown;
        bool laserActive;
        sf::Clock laserClock;
        sf::Sprite laserPowerUpLogo;

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
        void handle_start(std::optional<component::position>& pos);
        void handle_hitbox();

        void handleServerCommands();
        void handleMoveCommand(std::istringstream& iss);
        void handleMobSpawnCommand(std::istringstream& iss);
        void handleConnectCommand(std::istringstream& iss);
        void handleDisconnectCommand(std::istringstream& iss);
        void handleMovementCommands(std::istringstream& iss, GameAction action);
        void handleShootCommands(std::istringstream& iss);
        void handleSuperShootCommands(std::istringstream& iss);
        void handleLaserShootCommands(std::istringstream& iss);
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
        void handleWinCommand(std::istringstream& iss);
        void handleStartCommand(std::istringstream& iss);
        void handleScoreUpdateCommand(std::istringstream& iss);
        void handleGetLevelsCommand(std::istringstream& iss);
        void updatePlayerId();
        void update_hud();
        void displayRegistryInfo();
        sf::Text registryInfo;

        AnimationState animState;
        InputState inputState;

        Entity player;
        std::unordered_map<int, Entity> player_id_map;
        std::vector<std::string> levelFiles;
        bool wasMousePressed;
        sf::RectangleShape dropdownButton;
        sf::Text dropdownText;
        bool isDropdownOpen;
        std::vector<sf::RectangleShape> levelButtons;
        std::vector<sf::Text> levelTexts;
        int levelSelected;
        void initializeLevelSelector();
        void updateLevelSelector();
        void updateCurrentMap();
        sf::Text nb_players_text;

        struct PlayerInfo {
            bool isReady;
            int id;
            std::string name;
            int hp;
            sf::Text hpText;
        };
        std::vector<PlayerInfo> otherPlayers;
        int nb_player;

        //SOUNDS

        sf::Music menuMusic;
        sf::Music Game1Music;
        sf::Music BossMusic1;

        sf::Music Game2Music;
        sf::Music Game3Music;
        sf::Music Game4Music;

        sf::Music gameoverMusic;
        sf::Music winMusic;

        
        sf::SoundBuffer screamboss_buffer1;
        sf::Sound screamboss_sound1;

        sf::SoundBuffer buttonBuffer_click;
        sf::Sound buttonSound_click;

        sf::SoundBuffer powerupBuffer;
        sf::Sound powerupSound;

        sf::SoundBuffer shotBuffer;
        sf::Sound shotSound;

        sf::SoundBuffer superShotBuffer;
        sf::Sound superShotSound;

        sf::SoundBuffer laserBuffer;
        sf::Sound laserSound;

        sf::SoundBuffer joiningBuffer;
        sf::Sound joiningSound;

        sf::SoundBuffer explosionBuffer;
        sf::Sound explosionSound;

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
        sf::Shader blackAndWhiteShader;
        sf::RenderStates states;

        //GAMEOVER SCREEN
        sf::Sprite gameOverSprite;
        sf::Texture gameOverTexture;

        //WIN SCREEN
        sf::Sprite gameWinSprite;
        sf::Texture gameWinTexture;

        void playIntroAnimation();
        bool isDead;
        void updateAnimations();

        sf::Sprite save_replay;
        bool isReplay = false;

        struct AnimationLobby {
            float progress = 0.0f;
            bool isAnimating = true;
            float targetX = 0.0f;
            float targetY = 0.0f;
            float startX = 0.0f;
            float startY = 0.0f;
        };

        float scoreScale = 1.0f;
        sf::Clock scoreAnimClock;
        void handleLooseCommand(std::istringstream& iss);
        void display_lobby();
        std::vector<sf::Sprite> shipSprites;
        std::vector<sf::Vector2f> shipPositions;
        void load_levels();
        void handleBossSpawnCommand(std::istringstream& iss);
        void handleLevelEditorCommand(std::istringstream& iss);
        void handleChangeSkinCommand(std::istringstream& iss);
        std::vector<EntityPosition> entities;

        void startExplosionAt(float x, float y);
        void updateExplosions(float deltaSeconds);
        struct ExplosionAnimation {
            sf::Sprite sprite;
            float animationTimer = 0.0f;
            int currentFrame = 0;
            bool isActive = false;
            float x = 0.0f;
            float y = 0.0f;
            static constexpr int FRAME_WIDTH = 40;  // Ajuste selon ta spritesheet
            static constexpr int FRAME_HEIGHT = 33; // Ajuste selon ta spritesheet
            static constexpr float FRAME_DURATION = 0.15f;
            static constexpr int MAX_FRAMES = 6;    // Nombre total de frames dans l'animation
        };
        std::vector<ExplosionAnimation> activeExplosions;
        sf::Texture explosionTexture;
        sf::Clock clock_explosion;

         bool isFlashing = false;
        sf::Clock flashClock;
        float flashAlpha = 0.0f;
        sf::RectangleShape flashOverlay;

        sf::RectangleShape hudBackground;
        sf::RectangleShape scoreBackground;
        sf::RectangleShape PlayerHUD1;
        sf::RectangleShape PlayerHUD2;
        sf::RectangleShape PlayerHUD3;
        sf::RectangleShape PlayerHUD4;

        std::map<std::string, KeyBinding> keyBindings;
        std::vector<sf::RectangleShape> keyBindingButtons;
        sf::Text keyBindingTitle;
        void initializeKeyBindings();
        void handleKeyBindingChange(sf::Keyboard::Key newKey);


        sf::RectangleShape bossHealthBarBackground;
        sf::RectangleShape bossHealthBarFill;
        bool isBossActive = false;
        int currentBossHealth = 1000;
        int maxBossHealth = 1000;

};
