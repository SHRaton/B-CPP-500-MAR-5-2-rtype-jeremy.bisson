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
    reg.register_component<component::invincible>();
    reg.register_component<component::size>();

    med.register_game(this);
};

//===================================TIMERS===================================

void ServerGame::initTimers()
{
    spawn_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(20));
    setup_spawn_timer(*spawn_timer_);
    position_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::milliseconds(1));
    setup_position_timer(*position_timer_);
    conciliation_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(10));
    setup_conciliation_timer(*conciliation_timer_);

    //TODO: initialiser d'autres timers ici
    powerup_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(25));
    setup_powerup_timer(*powerup_timer_);

    collision_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
    setup_collision_timer(*collision_timer_);

    invincible_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
    setup_invincible_timer(*invincible_timer_);

    ia_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(15));
    setup_iaMobs(*ia_timer_);

    io_thread_ = std::thread([this]() {
            io_context_.run();
    });
}

void ServerGame::setup_position_timer(boost::asio::steady_timer& position_timer)
{
    position_timer.async_wait([this, &position_timer](const boost::system::error_code& ec) {
        if (!ec) {
            Systems::position_system(reg);
            position_timer.expires_at(position_timer.expiry() + std::chrono::milliseconds(1));
            setup_position_timer(position_timer);
        }
    });
}

void ServerGame::setup_conciliation_timer(boost::asio::steady_timer& conciliation_timer)
{
    conciliation_timer.async_wait([this, &conciliation_timer](const boost::system::error_code& ec) {
        if (!ec) {
            positionConciliation();
            conciliation_timer.expires_at(conciliation_timer.expiry() + std::chrono::seconds(3));
            setup_conciliation_timer(conciliation_timer);
        }
    });
}

void ServerGame::setup_collision_timer(boost::asio::steady_timer& collision_timer)
{
    collision_timer.async_wait([this, &collision_timer](const boost::system::error_code& ec) {
        if (!ec) {
            checkAllCollisions();
            collision_timer.expires_at(collision_timer.expiry() + std::chrono::milliseconds(10));
            setup_collision_timer(collision_timer);
        }
    });

}

void ServerGame::setup_invincible_timer(boost::asio::steady_timer& invincible_timer)
{
    invincible_timer.async_wait([this, &invincible_timer](const boost::system::error_code& ec) {
        if (!ec) {
            auto& invincibles = reg.get_components<component::invincible>();
            for (size_t i = 0; i < invincibles.size(); ++i) {
                if (invincibles[i].value().is_invincible) {
                    invincibles[i].value().is_invincible = false;
                }
            }
            invincible_timer.expires_at(invincible_timer.expiry() + std::chrono::seconds(1));
            setup_invincible_timer(invincible_timer);
        }
    });
}

void ServerGame::setup_iaMobs(boost::asio::steady_timer& ia_timer)
{
    std::cout << Colors::GREEN << "[Console] Setting up IA timer" << Colors::RESET << std::endl;
    
    try {
        ia_timer.async_wait([this, &ia_timer](const boost::system::error_code& ec) {
            if (!ec) {
                std::cout << Colors::BLUE << "[Console] IA Timer tick" << Colors::RESET << std::endl;
                
                auto& types = reg.get_components<component::type>();
                auto& velocities = reg.get_components<component::velocity>();
                
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dist(0, 1);
                
                for (size_t i = 0; i < types.size(); ++i) {
                    if (types[i].has_value() && types[i].value().type >= 10) {
                        int direction = dist(gen);
                        
                        if (direction == 0) {
                            velocities[i].value().vy = -1;
                        } else {
                            velocities[i].value().vy = 1;
                        }
                        
                        std::vector<std::string> params = {std::to_string(i)};
                        MediatorContext dummyContext;
                        handleMoves((direction == 0 ? "UP" : "DOWN"), dummyContext, params);
                    }
                }
                
                ia_timer.expires_from_now(std::chrono::milliseconds(300));
                setup_iaMobs(ia_timer);
            } else {
                std::cout << Colors::RED << "[Error] IA timer error: " << ec.message() << Colors::RESET << std::endl;
            }
        });
    } catch (const std::exception& e) {
        std::cout << Colors::RED << "[Error] Exception in setup_iaMobs: " << e.what() << Colors::RESET << std::endl;
    }
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

    // Utilisation de random modern de C++11
    std::random_device rd;  // Source d'entropie matérielle
    std::mt19937 gen(rd()); // Générateur Mersenne Twister

    // Distributions uniformes pour x et y
    std::uniform_int_distribution<> distX(0, 1800);
    std::uniform_int_distribution<> distY(0, 900);

    // Génération des coordonnées aléatoires
    int x = distX(gen);
    int y = distY(gen);

    Entity powerup = reg.spawn_entity();
    reg.emplace_component<component::position>(powerup, component::position{x, y});
    reg.emplace_component<component::type>(powerup, component::type{powerup_type});
    reg.emplace_component<component::size>(powerup, component::size{50, 50});

    std::vector<std::string> newParams;
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
        reg.emplace_component<component::velocity>(mob, component::velocity{-1, 0});
        reg.emplace_component<component::type>(mob, component::type{10});
        reg.emplace_component<component::size>(mob, component::size{100, 50});
    } else if (mob_type == 1) {
        reg.emplace_component<component::health>(mob, component::health{100});
        reg.emplace_component<component::damage>(mob, component::damage{40});
        reg.emplace_component<component::velocity>(mob, component::velocity{-1, 0});
        reg.emplace_component<component::type>(mob, component::type{11});
        reg.emplace_component<component::size>(mob, component::size{100, 50});

    } // rajouter d'autres types de mobs ici
    std::vector<std::string> newParams;
    newParams.push_back(std::to_string(mob_type));
    newParams.push_back(std::to_string(x));
    newParams.push_back(std::to_string(y));
    med.notify(Sender::GAME, "MOB_SPAWN", newParams);
}

void ServerGame::checkAllCollisions()
{
    auto& positions = reg.get_components<component::position>();
    auto& sizes = reg.get_components<component::size>();
    auto& types = reg.get_components<component::type>();
    auto& healths = reg.get_components<component::health>();
    //auto& invincibles = reg.get_components<component::invincible>();

    for (size_t i = 0; i < positions.size(); ++i) {

        for (size_t j = i + 1; j < positions.size(); ++j) {
            // if (invincibles.size() > i && invincibles.size() > j) {
                // auto &invincible1 = invincibles[i];
                // auto &invincible2 = invincibles[j];
                // if (invincibles[i].value().is_invincible || invincibles[j].value().is_invincible) {
                    // continue;
                    // }
            // }
            if (positions[i].has_value() == false || positions[j].has_value() == false) {
                std::cout << "No position for entity !!!!!!!!!!!!!!!!!!!!!!!!! " << i << std::endl;
                std::cout << "No position for entity !!!!!!!!!!!!!!!!!!!!!!!!! " << j << std::endl;
            }
            if (isColliding(positions[i].value(), positions[j].value(), sizes[i].value(), sizes[j].value())) {
                // std::cout << "Collision detected" << std::endl;
                if (types[i].value().type == 5 && types[j].value().type >= 10) { // MOB vs PLAYER
                    healths[i].value().hp -= 1000;
                    //reg.emplace_component<component::invincible>(Entity(i), component::invincible{true});
                    if (healths[i].value().hp <= 0) {
                        MediatorContext dummyContext;
                        handleDeath(dummyContext, std::vector<std::string>{std::to_string(i)});
                        reg.kill_entity(Entity(i));
                        checkAllCollisions();
                        return;
                    } else {
                        std::vector<std::string> collisionParams;
                        collisionParams.push_back(std::to_string(i));
                        collisionParams.push_back(std::to_string(10));
                        MediatorContext dummyContext;
                        handleColision(dummyContext, collisionParams);
                    }
                } else if (types[i].value().type >= 10 && types[j].value().type == 5) { // MOB vs PLAYER
                    healths[j].value().hp -= 1000;
                    //reg.emplace_component<component::invincible>(Entity(j), component::invincible{true});
                    if (healths[j].value().hp <= 0) {
                        MediatorContext dummyContext;
                        handleDeath(dummyContext, std::vector<std::string>{std::to_string(j)});
                        reg.kill_entity(Entity(j));
                        checkAllCollisions();
                        return;
                    } else {
                        std::vector<std::string> collisionParams;
                        collisionParams.push_back(std::to_string(j));
                        collisionParams.push_back(std::to_string(10));
                        MediatorContext dummyContext;
                        handleColision(dummyContext, collisionParams);
                    }
                } else if (types[i].value().type == 6 && types[j].value().type >= 10) { // BULLET vs MOB
                    // std::cout << "BULLET vs MOB" << std::endl;
                    healths[j].value().hp -= 1000;
                    // Mob
                    if(healths[j].value().hp <= 0){
                        MediatorContext dummyContext;
                        handleDeath(dummyContext, std::vector<std::string>{std::to_string(j)});
                        reg.kill_entity(Entity(j));
                        checkAllCollisions();
                        return;
                    }
                } else if (types[i].value().type >= 10 && types[j].value().type == 6) { // BULLET vs MOB
                    // std::cout << "BULLET vs MOB" << std::endl;
                    healths[i].value().hp -= 1000;
                    //Mob
                    if(healths[i].value().hp <= 0){
                        MediatorContext dummyContext;
                        handleDeath(dummyContext, std::vector<std::string>{std::to_string(i)});
                        reg.kill_entity(Entity(i));
                        checkAllCollisions();
                        return;
                    }
                } else if ((types[i].value().type == 0 || types[i].value().type == 1) && types[j].value().type == 5) { // PLAYER vs POWERUP
                    std::vector<std::string> collisionParams;
                    collisionParams.push_back(std::to_string(j));
                    collisionParams.push_back(std::to_string(types[i].value().type));
                    MediatorContext dummyContext;
                    handleColision(dummyContext, collisionParams);
                    handleDeath(dummyContext, std::vector<std::string>{std::to_string(i)});
                    // std::cout << "PLAYER vs POWERUP" << std::endl;
                    reg.kill_entity(Entity(i));
                    // std::cout << "Killed entity" << std::endl;
                    checkAllCollisions();
                    return;
                } else if ((types[j].value().type == 0 || types[j].value().type == 1) && types[i].value().type == 5) { // PLAYER vs POWERUP
                    std::vector<std::string> collisionParams;
                    collisionParams.push_back(std::to_string(i));
                    collisionParams.push_back(std::to_string(types[j].value().type));
                    MediatorContext dummyContext;
                    handleColision(dummyContext, collisionParams);
                    handleDeath(dummyContext, std::vector<std::string>{std::to_string(j)});
                    // std::cout << "PLAYER vs POWERUP" << std::endl;
                    reg.kill_entity(Entity(j));
                    // std::cout << "Killed entity" << std::endl;
                    checkAllCollisions();
                    return;
                }
                std::cout << "CNOollision detected" << std::endl;
            }
        }
    }
}

bool ServerGame::isColliding(const component::position& pos1, const component::position& pos2, const component::size& size1, const component::size& size2)
{
    bool x_collision = pos1.x < pos2.x + size2.width && pos2.x < pos1.x + size1.width;
    bool y_collision = pos1.y < pos2.y + size2.height && pos2.y < pos1.y + size1.height;
    return x_collision && y_collision;
}



//===================================COMMANDS=================================

void ServerGame::handleConnect(const MediatorContext& context, const std::vector<std::string>& params)
{
    Entity player = reg.spawn_entity();
    reg.emplace_component<component::position>(player, component::position{200, 500});
    reg.emplace_component<component::velocity>(player, component::velocity{0, 0});
    reg.emplace_component<component::controllable>(player, component::controllable{true});
    reg.emplace_component<component::health>(player, component::health{100});
    reg.emplace_component<component::damage>(player, component::damage{10});
    reg.emplace_component<component::type>(player, component::type{5});
    reg.emplace_component<component::size>(player, component::size{50, 50});

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
    reg.emplace_component<component::velocity>(bullet, component::velocity{1, 0});
    reg.emplace_component<component::type>(bullet, component::type{6});
    reg.emplace_component<component::size>(bullet, component::size{10, 10});

    std::vector<std::string> newParams;
    newParams.push_back(std::to_string(positions.x));
    newParams.push_back(std::to_string(positions.y));
    med.notify(Sender::GAME, "SHOOT", newParams, context);
}

void ServerGame::handleColision(const MediatorContext& context, const std::vector<std::string>& params)
{
    std::vector<std::string> newParams;
    newParams.push_back(params[0]);
    newParams.push_back(params[1]);
    med.notify(Sender::GAME, "COLISION", newParams, context);
}

void ServerGame::handleDeath(const MediatorContext& context, const std::vector<std::string>& params)
{
    std::vector<std::string> newParams;
    newParams.push_back(params[0]);
    med.notify(Sender::GAME, "DEATH", newParams, context);
}
