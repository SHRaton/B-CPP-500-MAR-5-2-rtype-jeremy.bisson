#include "Core.hpp"
#include "Utils.hpp"

Core::Core() :
        window(sf::VideoMode(1920, 1080), "R-Type"), player(0)
{
    // Start the Asio IO context in a separate thread
    loadAssets();
}

Core::~Core()
{
}

void Core::initialize_network(const std::string& server_ip, uint16_t port)
{
    network = std::make_unique<UDPNetworkClient>(server_ip, port);
}


void Core::gui(int argc, char **argv)
{
    utils.printTerminal(Color::GREEN, "Gui Started");
    gui_menu();
}

void Core::loadAssets()
{
    nb_player = 0;
    daltonismType = DaltonismType::NONE;

    renderTexture.create(window.getSize().x, window.getSize().y);

    texture_vaisseau0.loadFromFile("../ressources/sprites/vaisseau0.png");
    texture_vaisseau1.loadFromFile("../ressources/sprites/vaisseau1.png");
    texture_vaisseau2.loadFromFile("../ressources/sprites/vaisseau2.png");
    texture_vaisseau3.loadFromFile("../ressources/sprites/vaisseau3.png");


    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::health>();
    reg.register_component<component::damage>();
    reg.register_component<component::drawable>();
    reg.register_component<component::controllable>();
    reg.register_component<component::type>();

    sprites_menu = {
        {"background", Sprite("../ressources/background/background.png", true)},
        {"small_stars", Sprite("../ressources/background/small_stars.png", true, 1.0f, 20)},
        {"blueGalaxy", Sprite("../ressources/background/poudreBleu.png", true, 1.0f, 90)},
        {"rtype", Sprite("../ressources/background/rtype.png", false)},
        {"play", Sprite("../ressources/background/play.png", false, 1.0f, 50, "../ressources/background/play_hover.png")},
        {"quit", Sprite("../ressources/background/quit.png", false, 1.0f, 50, "../ressources/background/quit_hover2.png")},
        {"option", Sprite("../ressources/background/option.png", false, 1.0f, 50, "../ressources/background/option_hover.png")}
    };
    drawOrder_menu = {
        "background",
        "small_stars",
        "blueGalaxy",
        "rtype",
        "play",
        "quit",
        "option"
    };

    sprites_option = {
        {"option_background", Sprite("../ressources/background/option_background.png")},
        {"back", Sprite("../ressources/background/back.png")},
        {"none", Sprite("../ressources/background/none.png")},
        {"deuteranopy", Sprite("../ressources/background/deuteranopy.png")},
        {"tritanopy", Sprite("../ressources/background/tritanopy.png")},
        {"protanopy", Sprite("../ressources/background/protanopy.png")},
        {"is_active", Sprite("../ressources/background/is_active.png")}
    };
    drawOrder_option = {
        "option_background",
        "back",
        "none",
        "deuteranopy",
        "tritanopy",
        "protanopy",
        "is_active"
    };

    sprites_login = {
        {"background", Sprite("../ressources/background/background.png", true)},
        {"small_stars", Sprite("../ressources/background/small_stars.png", true, 1.0f, 20)},
        {"blueGalaxy", Sprite("../ressources/background/poudreBleu.png", true, 1.0f, 90)},
        {"rtype", Sprite("../ressources/background/rtype.png", false)},
        {"box_opacity", Sprite("../ressources/background/box_opacity2.png", false)},
        {"name", Sprite("../ressources/background/name.png", false)},
        {"ip", Sprite("../ressources/background/ip.png", false)},
        {"port", Sprite("../ressources/background/port.png", false)},
        {"connect", Sprite("../ressources/background/connect.png", false)}
    };
    drawOrder_login = {
        "background",
        "small_stars",
        "blueGalaxy",
        "rtype",
        "box_opacity",
        "name",
        "ip",
        "port",
        "connect"
    };

    sprites_game = {
        {"background", Sprite("../ressources/background/background.png", true, 4.5)},
        {"small_stars", Sprite("../ressources/background/small_stars.png", true, 2.0f, 20)},
    };
    drawOrder_game = {
        "background",
        "small_stars",
    };

    font.loadFromFile("../ressources/fonts/NicoMoji.ttf");
    str_name = "Raton";
    str_ip = "127.0.0.1";
    str_port = "8080";
    str_failed = "Connection Failed";

    failed_connection = 0;
    select_button = 0;

    fps = 200;
    baseSpeed = 1;
    shootCooldown = 0.0f;
    shootBar.setFillColor(sf::Color::Blue);
    shootBar.setPosition(100, 70);
    shootBar.setSize(sf::Vector2f(70, 5));

    soundVolume = 0.5f;
    //MUSIC MENU + LOGIN
    if (!menuMusic.openFromFile("../ressources/sounds/menu.wav")) {
        std::cout << "Error loading menu music" << std::endl;
    }
    menuMusic.setLoop(true);
    menuMusic.setVolume(soundVolume * 100.0);

    // MUSIC JEU
    if (!Game1Music.openFromFile("../ressources/sounds/game1.wav")) {
        std::cout << "Error loading menu music" << std::endl;
    }
    Game1Music.setLoop(true);
    Game1Music.setVolume(soundVolume * 100.0);

    // SONS
    if (!buttonBuffer_click.loadFromFile("../ressources/sounds/click.wav")) {
        std::cout << "Error loading button sound" << std::endl;
    }
    buttonSound_click.setBuffer(buttonBuffer_click);
    buttonSound_click.setVolume(soundVolume * 100.0);

    if (!powerupBuffer.loadFromFile("../ressources/sounds/click.wav")) {
        std::cout << "Error loading button sound" << std::endl;
    }
    powerupSound.setBuffer(powerupBuffer);
    powerupSound.setVolume(soundVolume * 100.0);
}
