#include "ServerGame.hpp"

ServerGame::ServerGame(Mediator &med) : med(med), lua()
{
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::os);

    // Exposer une fonction C++ à Lua
    lua.set_function("print_from_cpp", [](const std::string& message) {
        std::cout << "[Lua] " << message << std::endl;
    });

    // Exemple d'exposition d'une méthode pour manipuler l'IA
    lua.set_function("update_velocity", [&](size_t entityId, double vx, double vy) {
        std::cout << "Updating velocity for entity " << entityId << " to (" << vx << ", " << vy << ")" << std::endl;
        auto& velocities = reg.get_components<component::velocity>();
        if (entityId < velocities.size() && velocities[entityId]) {
            velocities[entityId]->vx = vx;
            velocities[entityId]->vy = vy;
            if (vx > 0) {
                handleMoves("RIGHT", MediatorContext(), {std::to_string(entityId)});
            } else if (vx < 0) {
                handleMoves("LEFT", MediatorContext(), {std::to_string(entityId)});
            } else {
                handleMoves("STOP_X", MediatorContext(), {std::to_string(entityId)});
            }
            if (vy > 0) {
                handleMoves("DOWN", MediatorContext(), {std::to_string(entityId)});
            } else if (vy < 0) {
                handleMoves("UP", MediatorContext(), {std::to_string(entityId)});
            } else {
                handleMoves("STOP_Y", MediatorContext(), {std::to_string(entityId)});
            }
        }
    });

    lua.set_function("spawn_bullet", [&](size_t entityId) {
        auto& positions = reg.get_components<component::position>();
        int x = positions[entityId].value().x + 50;
        int y = positions[entityId].value().y + 50;
        std::cout << "Spawning bullet for entity " << entityId << " at (" << x << ", " << y << ")" << std::endl;
        Entity bullet = reg.spawn_entity();
        reg.emplace_component<component::position>(bullet, component::position{x, y});
        reg.emplace_component<component::velocity>(bullet, component::velocity{5, 0});
        reg.emplace_component<component::type>(bullet, component::type{6});
        reg.emplace_component<component::size>(bullet, component::size{10, 10});
        std::vector<std::string> newParams;
        newParams.push_back(std::to_string(x));
        newParams.push_back(std::to_string(y));
        med.notify(Sender::GAME, "SHOOT", newParams, MediatorContext());
    });

    loadLuaScript("../src/lua/enemy_ai.lua");
    loadJson("../src/json/level1.json");

    std::cout << "Lua VM initialized!" << std::endl;

    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::drawable>();
    reg.register_component<component::controllable>();
    reg.register_component<component::health>();
    reg.register_component<component::damage>();
    reg.register_component<component::type>();
    reg.register_component<component::invincible>();
    reg.register_component<component::size>();
    reg.register_component<component::triple_shot>();
    reg.register_component<component::score>();
    reg.register_component<component::force>();

    state = GameState::LOBBY;
    med.register_game(this);
};

void ServerGame::loadLuaScript(const std::string& scriptPath) {
    try {
        lua.script_file(scriptPath);
        std::cout << "Lua script loaded: " << scriptPath << std::endl;
    } catch (const sol::error& e) {
        std::cerr << "Error loading Lua script: " << e.what() << std::endl;
    }
}

void ServerGame::loadJson(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Impossible de lire le fichier : " << filename << std::endl;
        return;
    }

    nlohmann::json levelData;
    file >> levelData;

    for (const auto& entity : levelData["entities"]) {
        JsonEntity jsonEntity;
        jsonEntity.type = entity["type"];
        jsonEntity.subtype = entity["subtype"];
        jsonEntity.x = entity["position"]["x"];
        jsonEntity.y = entity["position"]["y"];
        std::cout << "Entity: " << jsonEntity.type << " at (" << jsonEntity.x << ", " << jsonEntity.y << ")" << std::endl;

        if (entity.contains("behavior")) {
            jsonEntity.behavior = entity["behavior"];
            std::cout << "  Behavior: " << jsonEntity.behavior << std::endl;
        } else {
            jsonEntity.behavior = "";
            std::cout << "  No behavior" << std::endl;
        }
        allEntities.push_back(jsonEntity);
    }
    std::sort(allEntities.begin(), allEntities.end(), [](const JsonEntity& a, const JsonEntity& b) {
        return a.x < b.x;
    });
}


//===================================TIMERS===================================

void ServerGame::initTimers(bool isAi)
{
    position_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::milliseconds(1));
    setup_position_timer(*position_timer_);
    conciliation_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(10));
    setup_conciliation_timer(*conciliation_timer_);

    //TODO: initialiser d'autres timers ici

    collision_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
    setup_collision_timer(*collision_timer_);

    invincible_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::milliseconds(10));
    setup_invincible_timer(*invincible_timer_);

    ia_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(5));
    setup_iaMobs(*ia_timer_);

    if (isAi) {
        player_ia_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
        setup_ia_player(*player_ia_timer_);
    }

    triple_shot_expiration_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
    setup_triple_shot_expiration_timer(*triple_shot_expiration_timer_);

    force_expiration_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
    setup_force_expiration_timer(*force_expiration_timer_);

    force_shot_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
    setup_force_shot_timer(*force_shot_timer_);

    // Timer temporaire de win
    win_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(123));
    win_timer_->async_wait([this](const boost::system::error_code& ec) {
        if (!ec) {
            med.notify(Sender::GAME, "WIN", {}, MediatorContext());
            StopAllTimers();
            std::cout << "SHEEEEEESSSSSSHHH CEST LA WIN" << std::endl;
        }
    });

    game_over_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
    setup_game_over_timer(*game_over_timer_);
    std::cout << Colors::GREEN << "[Game] Game over timer initialized" << Colors::RESET << std::endl;

    io_thread_ = std::thread([this]() {
            io_context_.run();
    });
}

void ServerGame::StopAllTimers()
{
    position_timer_->cancel();
    conciliation_timer_->cancel();
    collision_timer_->cancel();
    invincible_timer_->cancel();
    ia_timer_->cancel();
    triple_shot_expiration_timer_->cancel();
    win_timer_->cancel();
    game_over_timer_->cancel();
}

void ServerGame::setup_game_over_timer(boost::asio::steady_timer& game_over_timer)
{
    game_over_timer.async_wait([this, &game_over_timer](const boost::system::error_code& ec) {
        if (!ec) {
            if (state == GameState::INGAME && !areAllPlayersDead()) {
                med.notify(Sender::GAME, "LOOSE", {}, MediatorContext());
                StopAllTimers();
                state = GameState::LOBBY;
                std::cout << Colors::RED << "[Game] All players are dead - Game Over!" << Colors::RESET << std::endl;
                return;
            }
            game_over_timer.expires_at(game_over_timer.expiry() + std::chrono::seconds(2));
            setup_game_over_timer(game_over_timer);
        } else {
            std::cout << Colors::RED << "[Error] Game over timer error: " << ec.message() << Colors::RESET << std::endl;
        }
    });
}

bool ServerGame::areAllPlayersDead()
{
    auto& types = reg.get_components<component::type>();
    auto& healths = reg.get_components<component::health>();
    bool playersExist = false;
    int playerCount = 0;
    int deadPlayerCount = 0;

    for (size_t i = 0; i < types.size(); ++i) {
        if (types[i].has_value() && types[i].value().type == 5) {
            playerCount++;
            playersExist = true;
        }
    }
    if (playerCount == 0) {
        return false;
    }

    return playersExist ;
}


void ServerGame::setup_position_timer(boost::asio::steady_timer& position_timer)
{
    position_timer.async_wait([this, &position_timer](const boost::system::error_code& ec) {
        if (!ec) {
            Systems::position_system(reg);
            //Utilisation d'un itérateur pour pouvoir supprimer des éléments en itérant
            for (auto it = allEntities.begin(); it != allEntities.end(); ) {
                it->x -= 0.5;
                if (it->x < 1900) {
                    if (it->type == "mob") {
                        spawnMob(*it);
                    } else if (it->type == "powerup") {
                        spawnPowerUp(*it);
                    }
                    it = allEntities.erase(it);
                } else {
                    ++it;
                }
            }

            position_timer.expires_at(position_timer.expiry() + std::chrono::milliseconds(10));
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
            auto now = std::chrono::steady_clock::now();

            for (size_t i = 0; i < invincibles.size(); ++i) {
                if (invincibles[i].has_value() && invincibles[i].value().is_invincible) {
                    if (now >= invincibles[i].value().expiration_time) {
                        invincibles[i].value().is_invincible = false;
                    }
                }
            }
            invincible_timer.expires_at(invincible_timer.expiry() + std::chrono::milliseconds(10));
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
                MediatorContext dummyContext;
                std::cout << Colors::BLUE << "[Console] IA Timer tick" << Colors::RESET << std::endl;

                auto& types = reg.get_components<component::type>();
                auto& velocities = reg.get_components<component::velocity>();

                for (size_t i = 0; i < types.size(); ++i) {

                    if (types[i].has_value() && types[i].value().type >= 10) {
                        Entity bullet = reg.spawn_entity();
                        auto const &positions = reg.get_components<component::position>()[i].value();
                        std::vector<std::string> newParams;
                        newParams.push_back(std::to_string(positions.x));
                        newParams.push_back(std::to_string(positions.y));

                        reg.emplace_component<component::position>(bullet, component::position{positions.x, positions.y});
                        reg.emplace_component<component::velocity>(bullet, component::velocity{-5, 0});
                        reg.emplace_component<component::type>(bullet, component::type{7});
                        reg.emplace_component<component::size>(bullet, component::size{10, 10});

                        med.notify(Sender::GAME, "MOB_SHOOT", newParams, dummyContext);
                        lua["enemy_ai"](i, positions.x, positions.y); // Appel Lua
                    }
                }
                ia_timer.expires_from_now(std::chrono::milliseconds(500));
                setup_iaMobs(ia_timer);
            } else {
                std::cout << Colors::RED << "[Error] IA timer error: " << ec.message() << Colors::RESET << std::endl;
            }
        });
    } catch (const std::exception& e) {
        std::cout << Colors::RED << "[Error] Exception in setup_iaMobs: " << e.what() << Colors::RESET << std::endl;
    }
}

void ServerGame::setup_ia_player(boost::asio::steady_timer& player_ia_timer)
{
    std::cout << Colors::GREEN << "[Console] Setting up IA player timer" << Colors::RESET << std::endl;

    try {
        player_ia_timer.async_wait([this, &player_ia_timer](const boost::system::error_code& ec) {
            if (!ec) {
                MediatorContext dummyContext;
                auto const &positions = reg.get_components<component::position>()[1].value();
                lua["player_ai"](1, positions.x, positions.y); // Appel Lua

                player_ia_timer.expires_from_now(std::chrono::milliseconds(1000));
                setup_ia_player(player_ia_timer);
            } else {
                std::cout << Colors::RED << "[Error] IA player timer error: " << ec.message() << Colors::RESET << std::endl;
            }
        });
    } catch (const std::exception& e) {
        std::cout << Colors::RED << "[Error] Exception in setup_ia_player: " << e.what() << Colors::RESET << std::endl;
    }
}

void ServerGame::setup_triple_shot_expiration_timer(boost::asio::steady_timer& triple_shot_timer)
{
    triple_shot_timer.async_wait([this, &triple_shot_timer](const boost::system::error_code& ec) {
        if (!ec) {
            checkTripleShotExpiration();
            triple_shot_timer.expires_at(triple_shot_timer.expiry() + std::chrono::seconds(1));
            setup_triple_shot_expiration_timer(triple_shot_timer);
        }
    });
}

void ServerGame::setup_force_expiration_timer(boost::asio::steady_timer& force_timer)
{
    force_timer.async_wait([this, &force_timer](const boost::system::error_code& ec) {
        if (!ec) {
            checkForceExpiration();
            force_timer.expires_at(force_timer.expiry() + std::chrono::seconds(1));
            setup_force_expiration_timer(force_timer);
        }
    });
}

void ServerGame::setup_force_shot_timer(boost::asio::steady_timer& force_shot_timer)
{
    force_shot_timer.async_wait([this, &force_shot_timer](const boost::system::error_code& ec) {
        if (!ec) {
            auto &force = reg.get_components<component::force>();
            for (size_t i = 0; i < force.size(); ++i) {
                if (force[i].has_value() && force[i].value().is_active) {
                    Entity bullet = reg.spawn_entity();
                    auto const &positions = reg.get_components<component::position>()[i].value();
                    std::vector<std::string> newParams;
                    newParams.push_back(std::to_string(positions.x));
                    newParams.push_back(std::to_string(positions.y));
                    reg.emplace_component<component::position>(bullet, component::position{positions.x, positions.y});
                    if (force[i].value().is_front == 0) {  // Ajout des parenthèses après if
                        reg.emplace_component<component::velocity>(bullet, component::velocity{5, 0});
                    } else {
                        reg.emplace_component<component::velocity>(bullet, component::velocity{-5, 0});
                    }
                    reg.emplace_component<component::type>(bullet, component::type{8});
                    reg.emplace_component<component::size>(bullet, component::size{10, 10});
                    med.notify(Sender::GAME, "SHOOT", newParams, MediatorContext());
                }
            }
            force_shot_timer.expires_at(force_shot_timer.expiry() + std::chrono::milliseconds(10));
            setup_force_shot_timer(force_shot_timer);
        }
    });
}

void ServerGame::positionConciliation()
{
    auto &positions = reg.get_components<component::position>();
    for (size_t i =  0; i < positions.size();)
    {
        if (positions[i].value().x < -100 || positions[i].value().x > 2000)
        {
            handleDeath(MediatorContext(), std::vector<std::string>{std::to_string(i)});
            reg.kill_entity(Entity(i));
            continue;
        }
        //showAllEnityAlive();
        if (positions[i])
        {
                std::vector<std::string> newParams;
                newParams.push_back(std::to_string(i));
                newParams.push_back(std::to_string(positions[i].value().x));
                newParams.push_back(std::to_string(positions[i].value().y));
                med.notify(Sender::GAME, "MOVE", newParams);
        }
        ++i;
    }
}

void ServerGame::showAllEnityAlive()
{
    auto &positions = reg.get_components<component::position>();
    auto &types = reg.get_components<component::type>();
    for (size_t i = 0; i < positions.size(); ++i)
    {
        if (positions[i])
        {
            std::cout << "Entity " << types[i].value().type << " is alive." << std::endl;
        }
    }
}


void ServerGame::spawnPowerUp(JsonEntity entity)
{
    std::cout << "Spawning powerup" << std::endl;
    int x = rand() % 1500 + 300;
    int y = entity.y;
    int type = 0;
    if (entity.subtype == "heal") {
        type = 0;
    } else if (entity.subtype == "triple_shoot") {
        type = 1;
    } else if (entity.subtype == "laser") {
        type = 2;
    } // rajouter d'autres types de powerups ici

    Entity powerup = reg.spawn_entity();
    reg.emplace_component<component::position>(powerup, component::position{x, y});
    reg.emplace_component<component::type>(powerup, component::type{type});
    reg.emplace_component<component::size>(powerup, component::size{50, 50});

    std::vector<std::string> newParams;
    newParams.push_back(std::to_string(type));
    newParams.push_back(std::to_string(x));
    newParams.push_back(std::to_string(y));
    med.notify(Sender::GAME, "POWERUP_SPAWN", newParams);
}

void ServerGame::spawnMob(JsonEntity entity)
{
    std::cout << "Spawning mob" << std::endl;
    Entity mob = reg.spawn_entity();
    int x = entity.x;
    int y = entity.y;
    int type = 0;
    if (entity.subtype == "enemy1") {
        type = 0;
    } else if (entity.subtype == "enemy2") {
        type = 1;
    } // rajouter d'autres types de mobs ici
    reg.emplace_component<component::position>(mob, component::position{x, y});
    if (type == 0) {
        reg.emplace_component<component::health>(mob, component::health{300});
        reg.emplace_component<component::damage>(mob, component::damage{10});
        reg.emplace_component<component::velocity>(mob, component::velocity{-5, 0});
        reg.emplace_component<component::type>(mob, component::type{10});
        reg.emplace_component<component::size>(mob, component::size{100, 50});
    } else if (type == 1) {
        reg.emplace_component<component::health>(mob, component::health{100});
        reg.emplace_component<component::damage>(mob, component::damage{40});
        reg.emplace_component<component::velocity>(mob, component::velocity{-5, 0});
        reg.emplace_component<component::type>(mob, component::type{11});
        reg.emplace_component<component::size>(mob, component::size{100, 50});
    }
    std::vector<std::string> newParams;
    newParams.push_back(std::to_string(type));
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
    auto& triple_shots = reg.get_components<component::triple_shot>();
    auto& invincibles = reg.get_components<component::invincible>();
    auto& force = reg.get_components<component::force>();

    for (size_t i = 0; i < positions.size(); ++i) {

        for (size_t j = i + 1; j < positions.size(); ++j) {
            if (types[i].has_value() && types[j].has_value()) {
                // Si l'un des deux est un joueur, on vérifie son invincibilité
                if ((types[i].value().type == 5 && invincibles[i].has_value() && invincibles[i].value().is_invincible) ||
                    (types[j].value().type == 5 && invincibles[j].has_value() && invincibles[j].value().is_invincible)) {
                    continue;
                }
            }

            if (positions[i].has_value() == false || positions[j].has_value() == false) {
                std::cout << "No position for entity !!!!!!!!!!!!!!!!!!!!!!!!! " << i << std::endl;
                std::cout << "No position for entity !!!!!!!!!!!!!!!!!!!!!!!!! " << j << std::endl;
            }
            if (isColliding(positions[i].value(), positions[j].value(), sizes[i].value(), sizes[j].value())) {
                if (types[i].value().type == 5 && types[j].value().type >= 10) { // MOB vs PLAYER
                    healths[i].value().hp -= 50;
                    invincibles[i].value().is_invincible = true;
                    invincibles[i].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(1);
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
                    healths[j].value().hp -= 50;
                    invincibles[j].value().is_invincible = true;
                    invincibles[j].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(1);
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
                } else if (types[j].value().type == 7 && types[i].value().type == 5) { // MOB_BULLET vs PLAYER
                    healths[i].value().hp -= 30;
                    invincibles[i].value().is_invincible = true;
                    invincibles[i].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(1);
                    if (healths[i].value().hp <= 0) {
                        MediatorContext dummyContext;
                        handleDeath(dummyContext, std::vector<std::string>{std::to_string(i)});
                        reg.kill_entity(Entity(i));
                        checkAllCollisions();
                        return;
                    } else {
                        std::vector<std::string> collisionParams;
                        collisionParams.push_back(std::to_string(i));
                        collisionParams.push_back(std::to_string(7));
                        MediatorContext dummyContext;
                        handleColision(dummyContext, collisionParams);
                    }
                } else if (types[i].value().type == 7 && types[j].value().type == 5) { // MOB_BULLET vs PLAYER
                    healths[j].value().hp -= 30;
                    invincibles[j].value().is_invincible = true;
                    invincibles[j].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(1);
                    if (healths[j].value().hp <= 0) {
                        MediatorContext dummyContext;
                        handleDeath(dummyContext, std::vector<std::string>{std::to_string(j)});
                        reg.kill_entity(Entity(j));
                        checkAllCollisions();
                        return;
                    } else {
                        std::vector<std::string> collisionParams;
                        collisionParams.push_back(std::to_string(j));
                        collisionParams.push_back(std::to_string(7));
                        MediatorContext dummyContext;
                        handleColision(dummyContext, collisionParams);
                    }
                } else if (types[i].value().type == 6 && types[j].value().type >= 10) { // BULLET vs MOB
                    healths[j].value().hp -= 1000;
                    for (size_t k = 0; k < types.size(); ++k) {
                        if (types[k].has_value() && types[k].value().type == 5) {
                            auto& scores = reg.get_components<component::score>();
                            if (scores[k].has_value()) {
                                scores[k].value().value += 10;
                                std::vector<std::string> scoreParams = {
                                    std::to_string(k),
                                    std::to_string(scores[k].value().value)
                                };
                                med.notify(Sender::GAME, "SCORE_UPDATE", scoreParams, MediatorContext());
                            }
                        }
                    }
                    // Mob
                    if(healths[j].value().hp <= 0){
                        MediatorContext dummyContext;
                        handleDeath(dummyContext, std::vector<std::string>{std::to_string(j)});
                        reg.kill_entity(Entity(j));
                        checkAllCollisions();
                        return;
                    }
                } else if (types[i].value().type >= 10 && types[j].value().type == 6) { // BULLET vs MOB
                    healths[i].value().hp -= 1000;
                    for (size_t k = 0; k < types.size(); ++k) {
                        if (types[k].has_value() && types[k].value().type == 5) {
                            auto& scores = reg.get_components<component::score>();
                            if (scores[k].has_value()) {
                                scores[k].value().value += 10;
                                std::vector<std::string> scoreParams = {
                                    std::to_string(k),
                                    std::to_string(scores[k].value().value)
                                };
                                med.notify(Sender::GAME, "SCORE_UPDATE", scoreParams, MediatorContext());
                            }
                        }
                    }
                    // Mob
                    if(healths[i].value().hp <= 0){
                        MediatorContext dummyContext;
                        handleDeath(dummyContext, std::vector<std::string>{std::to_string(i)});
                        reg.kill_entity(Entity(i));
                        checkAllCollisions();
                        return;
                    }
                } else if ((types[i].value().type == 0 || types[i].value().type == 1 || types[i].value().type == 2) && types[j].value().type == 5) { // PLAYER vs POWERUP
                    if (types[i].value().type == 0) {
                        triple_shots[j].value().is_active = true;
                        triple_shots[j].value().activation_time = std::chrono::steady_clock::now();
                    } else if (types[i].value().type == 1) { 
                        healths[j].value().hp += 10;
                    } else if (types[i].value().type == 2) {
                        //Caractéristiques principales :
                        // tirer en avant, en arriere, bloque les balles ennemies
                        invincibles[j].value().is_invincible = true;
                        force[j].value().is_active = true;
                        force[j].value().is_front = 0;
                        force[j].value().activation_time = std::chrono::steady_clock::now();
                    } 
                    std::vector<std::string> collisionParams;
                    collisionParams.push_back(std::to_string(j));
                    collisionParams.push_back(std::to_string(types[i].value().type));
                    MediatorContext dummyContext;
                    handleColision(dummyContext, collisionParams);
                    handleDeath(dummyContext, std::vector<std::string>{std::to_string(i)});
                    reg.kill_entity(Entity(i));
                    checkAllCollisions();
                    return;
                } else if ((types[j].value().type == 0 || types[j].value().type == 1 || types[j].value().type == 2) && types[i].value().type == 5) { // PLAYER vs POWERUP
                    if (types[j].value().type == 0) {
                        triple_shots[i].value().is_active = true;
                        triple_shots[i].value().activation_time = std::chrono::steady_clock::now();
                    } else if (types[j].value().type == 1) {
                        healths[i].value().hp += 10;
                    } else if (types[j].value().type == 2) {
                        invincibles[i].value().is_invincible = true;
                        force[i].value().is_active = true;
                        force[i].value().is_front = 0;
                        force[i].value().activation_time = std::chrono::steady_clock::now();
                    }
                    std::vector<std::string> collisionParams;
                    collisionParams.push_back(std::to_string(i));
                    collisionParams.push_back(std::to_string(types[j].value().type));
                    MediatorContext dummyContext;
                    handleColision(dummyContext, collisionParams);
                    handleDeath(dummyContext, std::vector<std::string>{std::to_string(j)});
                    reg.kill_entity(Entity(j));
                    checkAllCollisions();
                    return;
                }
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


void ServerGame::checkTripleShotExpiration()
{
    auto& triple_shots = reg.get_components<component::triple_shot>();
    auto now = std::chrono::steady_clock::now();

    for (size_t i = 0; i < triple_shots.size(); ++i) {
        if (triple_shots[i] && triple_shots[i].value().is_active) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                now - triple_shots[i].value().activation_time
            ).count();

            if (duration >= 10) {
                // Désactiver le power-up
                triple_shots[i].value().is_active = false;
            }
        }
    }
}

void ServerGame::checkForceExpiration()
{
    auto& forces = reg.get_components<component::force>();
    auto now = std::chrono::steady_clock::now();

    for (size_t i = 0; i < forces.size(); ++i) {
        if (forces[i] && forces[i].value().is_active) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                now - forces[i].value().activation_time
            ).count();

            if (duration >= 4) {
                // Désactiver le power-up
                forces[i].value().is_active = false;
            }
        }
    }
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
    reg.emplace_component<component::triple_shot>(player, component::triple_shot{false, {}});
    reg.emplace_component<component::invincible>(player, component::invincible{false});
    reg.emplace_component<component::score>(player, component::score{0});
    reg.emplace_component<component::force>(player, component::force{false, {}});

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
    auto& controllables = reg.get_components<component::controllable>();
    size_t entityId = std::stoi(params[0]);

    for (size_t i = 0; i < controllables.size(); ++i) {
        if (i == entityId) {
            std::vector<std::string> disconnectParams = {std::to_string(i)};
            med.notify(Sender::GAME, "DISCONNECT", disconnectParams, context);
            reg.kill_entity(Entity(i));
            std::cout << "Joueur " << i << " s'est déconnecté" << std::endl;
            break;
        }
    }
}
void ServerGame::handleMoves(const std::string& action, const MediatorContext& context, const std::vector<std::string>& params)
{
    if (state == GameState::LOBBY) {
        return;
    }
    try {
        if (params.empty()) {
            throw std::runtime_error("No params");
        }
        size_t entityId = std::stoi(params[0]);
        auto& velocities = reg.get_components<component::velocity>();

        // Modifier la vélocité selon l'action
        auto& velocity = velocities[entityId].value();
        if (action == "UP") {
            velocity.vy = -5;
        } else if (action == "DOWN") {
            velocity.vy = 5;
        } else if (action == "LEFT") {
            velocity.vx = -5;
        } else if (action == "RIGHT") {
            velocity.vx = 5;
        } else if (action == "STOP_Y") {
            velocity.vy = 0;
        } else if (action == "STOP_X") {
            velocity.vx = 0;
        }

        // Notifier seulement si l'entité existe toujours
        med.notify(Sender::GAME, action, params, context);

    } catch(const std::exception& e) {
        std::cout << Colors::RED << "[Error] Exception in handleMoves: " << e.what() << Colors::RESET << std::endl;
        return;
    }
}

void ServerGame::handleStart(const MediatorContext& context, const std::vector<std::string>& params)
{
    if (state == GameState::INGAME) {
        return;
    }
    bool isAI = 0;
    if (reg.get_components<component::controllable>().size() == 1){
        handleConnect(MediatorContext(), params);
        std::cout << "AI started" << std::endl;
        loadLuaScript("../src/lua/player_ai.lua");
        isAI = 1;
    }

    std::cout << "Game started" << std::endl;
    state = GameState::INGAME;
    initTimers(isAI);
    med.notify(Sender::GAME, "START", params, context);
}

void ServerGame::handleShoot(const MediatorContext& context, const std::vector<std::string>& params)
{
    if (state == GameState::LOBBY) {
        return;
    }
    try {
        params.size() == 0 ? throw std::runtime_error("No params") : 0;
        int player_id = std::stoi(params[0]);
        auto const &positions = reg.get_components<component::position>()[std::stoi(params[0])].value();
        auto& triple_shots = reg.get_components<component::triple_shot>();

        if (triple_shots.size() > player_id && triple_shots[player_id].value().is_active) {
            // Tir multiple
            struct MissileConfig {
                int y_offset;
                int vx;
                int vy;
            };

            std::vector<MissileConfig> missiles = {
                {-20, 1, -1},   // Missile vers le haut
                {0, 1, 0},      // Missile droit
                {20, 1, 1}      // Missile vers le bas
            };

            for (const auto& missile : missiles) {
                Entity bullet = reg.spawn_entity();
                std::vector<std::string> newParams;
                newParams.push_back(std::to_string(positions.x));
                newParams.push_back(std::to_string(positions.y + missile.y_offset));
                reg.emplace_component<component::position>(bullet, component::position{positions.x, positions.y + missile.y_offset});
                reg.emplace_component<component::velocity>(bullet, component::velocity{5, 0});
                reg.emplace_component<component::type>(bullet, component::type{6});
                reg.emplace_component<component::size>(bullet, component::size{10, 10});
                med.notify(Sender::GAME, "SHOOT", newParams, context);
            }
        } else {
            // Tir normal
            Entity bullet = reg.spawn_entity();
            std::vector<std::string> newParams;
            newParams.push_back(std::to_string(positions.x));
            newParams.push_back(std::to_string(positions.y));
            reg.emplace_component<component::position>(bullet, component::position{positions.x, positions.y});
            reg.emplace_component<component::velocity>(bullet, component::velocity{5, 0});
            reg.emplace_component<component::type>(bullet, component::type{6});
            reg.emplace_component<component::size>(bullet, component::size{10, 10});
            med.notify(Sender::GAME, "SHOOT", newParams, context);
        }
    } catch(const std::exception& e) {
        std::cout << Colors::RED << "[Error] Exception in handleShoot: " << e.what() << Colors::RESET << std::endl;
        return;
    }
}

void ServerGame::handleColision(const MediatorContext& context, const std::vector<std::string>& params)
{
    try {
        params.size() == 0 ? throw std::runtime_error("No params") : 0;
        std::vector<std::string> newParams;
        newParams.push_back(params[0]);
        newParams.push_back(params[1]);
        med.notify(Sender::GAME, "COLISION", newParams, context);
    } catch(const std::exception& e) {
        std::cout << Colors::RED << "[Error] Exception in handleColision: " << e.what() << Colors::RESET << std::endl;
        return;
    }
}

void ServerGame::handleDeath(const MediatorContext& context, const std::vector<std::string>& params)
{
    try {
        params.size() == 0 ? throw std::runtime_error("No params") : 0;
        std::vector<std::string> newParams;
        newParams.push_back(params[0]);
        med.notify(Sender::GAME, "DEATH", newParams, context);
     } catch(const std::exception& e) {
        std::cout << Colors::RED << "[Error] Exception in handleDeath: " << e.what() << Colors::RESET << std::endl;
        return;
    }
}
