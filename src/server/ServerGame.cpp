#include "ServerGame.hpp"

ServerGame::ServerGame(Mediator &med) : med(med)
{
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::drawable>();
    reg.register_component<component::controllable>();
    reg.register_component<component::health>();
    reg.register_component<component::damage>();
    reg.register_component<component::type>();

    med.register_game(this);
};

//===================================TIMERS===================================

void ServerGame::initTimers()
{
    spawn_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(60));
    setup_spawn_timer(*spawn_timer_);
    position_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::milliseconds(1));
    setup_position_timer(*position_timer_);
    conciliation_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(10));
    setup_conciliation_timer(*conciliation_timer_);

    //TODO: initialiser d'autres timers ici
    powerup_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(60));
    setup_powerup_timer(*powerup_timer_);

    io_thread_ = std::thread([this]() {
            io_context_.run();
    });
}

void ServerGame::setup_position_timer(boost::asio::steady_timer& position_timer)
{
    position_timer.async_wait([this, &position_timer](const boost::system::error_code& ec) {
        if (!ec) {
            Systems::position_system(reg);
            position_timer.expires_from_now(std::chrono::milliseconds(1));
            setup_position_timer(position_timer);
        }
    });
}

void ServerGame::setup_conciliation_timer(boost::asio::steady_timer& conciliation_timer)
{
    conciliation_timer.async_wait([this, &conciliation_timer](const boost::system::error_code& ec) {
        if (!ec) {
            positionConciliation();
            conciliation_timer.expires_from_now(std::chrono::seconds(3));
            setup_conciliation_timer(conciliation_timer);
        }
    });
}

void ServerGame::positionConciliation()
{
    auto &positions = reg.get_components<component::position>();
    for (size_t i = 0; i < positions.size(); ++i)
    {
        if (positions[i])
        {
            std::vector<std::string> newParams;
            newParams.push_back(std::to_string(i));
            newParams.push_back(std::to_string(positions[i].value().x));
            newParams.push_back(std::to_string(positions[i].value().y));
            med.notify(Sender::GAME, "MOVE", newParams);
        }
    }
}

void ServerGame::setup_spawn_timer(boost::asio::steady_timer& spawn_timer)
{
    std::cout << "Setting up spawn timer" << std::endl;
    spawn_timer.async_wait([this, &spawn_timer](const boost::system::error_code& ec) {
        if (!ec) {
            spawnMob(rand() % 2); // Choix aléatoire du type de mob
            spawn_timer.expires_from_now(std::chrono::seconds(20)); //TODO: si on veut changer le temps de spawn
            setup_spawn_timer(spawn_timer);
        }
    });
}

void ServerGame::setup_powerup_timer(boost::asio::steady_timer& powerup_timer)
{
    std::cout << "Setting up powerup timer" << std::endl;
    powerup_timer.async_wait([this, &powerup_timer](const boost::system::error_code& ec) {
        if (!ec) {
            spawnPowerUp(rand() % 2);
            powerup_timer.expires_from_now(std::chrono::seconds(60));
            setup_powerup_timer(powerup_timer);
        }
    });
}

void ServerGame::spawnPowerUp(int powerup_type)
{
    std::cout << "Spawning powerup" << std::endl;
    Entity powerup = reg.spawn_entity();
    int x = rand() % 1800;
    int y = rand() % 900;
    reg.emplace_component<component::position>(powerup, component::position{x, y});
    reg.emplace_component<component::type>(powerup, component::type{powerup_type});
    
    std::vector<std::string> newParams;
    newParams.push_back(std::to_string(powerup));
    newParams.push_back(std::to_string(powerup_type));
    newParams.push_back(std::to_string(x));
    newParams.push_back(std::to_string(y));
    med.notify(Sender::GAME, "POWERUP_SPAWN", newParams);
}

void ServerGame::spawnMob(int mob_type)
{
    std::cout << "Spawning mob" << std::endl;
    Entity mob = reg.spawn_entity();
    int x = 1800;
    int y = rand() % 900;
    reg.emplace_component<component::position>(mob, component::position{x, y});
    if (mob_type == 0) {
        reg.emplace_component<component::health>(mob, component::health{300});
        reg.emplace_component<component::damage>(mob, component::damage{10});
        reg.emplace_component<component::velocity>(mob, component::velocity{5, 5});
    } else if (mob_type == 1) {
        reg.emplace_component<component::health>(mob, component::health{100});
        reg.emplace_component<component::damage>(mob, component::damage{40});
        reg.emplace_component<component::velocity>(mob, component::velocity{10, 10});
    } // rajouter d'autres types de mobs ici
    std::vector<std::string> newParams;
    newParams.push_back(std::to_string(mob));
    newParams.push_back(std::to_string(mob_type));
    newParams.push_back(std::to_string(x));
    newParams.push_back(std::to_string(y));
    med.notify(Sender::GAME, "MOB_SPAWN", newParams);
}




//===================================COMMANDS=================================

void ServerGame::handleConnect(const MediatorContext& context, const std::vector<std::string>& params)
{
    Entity player = reg.spawn_entity();
    reg.emplace_component<component::position>(player, component::position{0, 0});
    reg.emplace_component<component::velocity>(player, component::velocity{0, 0});
    reg.emplace_component<component::controllable>(player, component::controllable{true});
    reg.emplace_component<component::health>(player, component::health{100});
    reg.emplace_component<component::damage>(player, component::damage{10});

    std::vector<std::string> newParams;

    auto &controllables = reg.get_components<component::controllable>();
    for (size_t i = 0; i < controllables.size(); ++i)
    {
        auto &controllable = controllables[i];
        if (controllable)
        {
            if (controllable.value().is_controllable)
            {
                newParams.push_back(std::to_string(i));
            }
        }
    }


    med.notify(Sender::GAME, "CONNECT", newParams, context);
}

void ServerGame::handleDisconnect(const MediatorContext& context, const std::vector<std::string>& params)
{
    boost::asio::ip::udp::endpoint client = context.client;
    std::cout << "Un joueur s'est déconnecté" << std::endl;
}

void ServerGame::handleMoves(const std::string& action, const MediatorContext& context, const std::vector<std::string>& params)
{
    if (action == "UP"){
        reg.get_components<component::velocity>()[std::stoi(params[0])].value().vy = -1;
    } else if (action == "DOWN"){
        reg.get_components<component::velocity>()[std::stoi(params[0])].value().vy = 1;
    } else if (action == "LEFT"){
        reg.get_components<component::velocity>()[std::stoi(params[0])].value().vx = -1;
    } else if (action == "RIGHT"){
        reg.get_components<component::velocity>()[std::stoi(params[0])].value().vx = 1;
    } else if (action == "STOP_Y"){
        reg.get_components<component::velocity>()[std::stoi(params[0])].value().vy = 0;
    } else if (action == "STOP_X"){
        reg.get_components<component::velocity>()[std::stoi(params[0])].value().vx = 0;
    }
    med.notify(Sender::GAME, action, params, context);
}

void ServerGame::handleShoot(const MediatorContext& context, const std::vector<std::string>& params)
{
    Entity bullet = reg.spawn_entity();
    auto const &positions = reg.get_components<component::position>()[std::stoi(params[0])].value();
    reg.emplace_component<component::position>(bullet, component::position{positions.x, positions.y});
    reg.emplace_component<component::velocity>(bullet, component::velocity{10, 0});

    std::vector<std::string> newParams;
    newParams.push_back(std::to_string(positions.x));
    newParams.push_back(std::to_string(positions.y));
    med.notify(Sender::GAME, "SHOOT", newParams, context);
}