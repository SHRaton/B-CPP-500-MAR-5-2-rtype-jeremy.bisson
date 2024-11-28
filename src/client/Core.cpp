#include "Core.hpp"
#include "Utils.hpp"

Core::Core() :
        window(sf::VideoMode(1920, 1080), "R-Type")
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
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::drawable>();
    reg.register_component<component::controllable>();

    // Création du joueur
    Entity player = reg.spawn_entity();
    reg.emplace_component<component::position>(player, component::position{0, 0});
    reg.emplace_component<component::velocity>(player, component::velocity{0, 0});
    reg.emplace_component<component::drawable>(player, component::drawable{"./ressources/sprites/jinx.png"});
    reg.emplace_component<component::controllable>(player, component::controllable{true});

    // Création de l'ennemi
    Entity enemy = reg.spawn_entity();
    reg.emplace_component<component::position>(enemy, component::position{100, 100});
    reg.emplace_component<component::velocity>(enemy, component::velocity{1, 1});
    reg.emplace_component<component::drawable>(enemy, component::drawable{"./ressources/sprites/jinx.png"});
    reg.emplace_component<component::controllable>(enemy, component::controllable{false});


    sprites_menu = {
        {"background", Sprite("../ressources/background/background.png", true)},
        {"small_stars", Sprite("../ressources/background/small_stars.png", true, 1.0f, 20)},
        {"poudreBleu", Sprite("../ressources/background/poudreBleu.png", true, 1.0f, 90)},
        {"rtype", Sprite("../ressources/background/rtype.png", false)},
        {"play", Sprite("../ressources/background/play.png", false, 1.0f, 50, "../ressources/background/play_hover.png")},
        {"quit", Sprite("../ressources/background/quit.png", false, 1.0f, 50, "../ressources/background/quit_hover2.png")}
    };
    drawOrder_menu = {
        "background",
        "small_stars",
        "poudreBleu",
        "rtype",
        "play",
        "quit"
    };

    sprites_login = {
        {"background", Sprite("../ressources/background/background.png", true)},
        {"small_stars", Sprite("../ressources/background/small_stars.png", true, 1.0f, 20)},
        {"poudreBleu", Sprite("../ressources/background/poudreBleu.png", true, 1.0f, 90)},
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
        "poudreBleu",
        "rtype",
        "box_opacity",
        "name",
        "ip",
        "port",
        "connect"
    };
    font.loadFromFile("../ressources/fonts/NicoMoji.ttf");
    str_name = "Raton";
    str_ip = "";
    str_port = "";
    str_failed = "nooby failed to connect";

    failed_connection = 0;
    select_button = 0;
}