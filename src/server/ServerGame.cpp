#include "ServerGame.hpp"

ServerGame::ServerGame(Mediator &med) : med(med), lua()
{
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::os);

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
    loadLuaScript("../src/lua/enemy_ai2.lua");
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
    reg.register_component<component::laser_shot>();
    reg.register_component<component::super_shot>();
    reg.register_component<component::score>();
    reg.register_component<component::force>();
    reg.register_component<component::bits>();

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

    powerup_expiration_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::milliseconds(10));
    setup_powerup_expiration_timer(*powerup_expiration_timer_);

    ia_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(5));
    setup_iaMobs(*ia_timer_);

    if (isAi) {
        player_ia_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
        setup_ia_player(*player_ia_timer_);
    }

    force_shot_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
    setup_force_shot_timer(*force_shot_timer_);

    bits_shot_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
    setup_bits_shot_timer(*bits_shot_timer_);

    laser_shot_expiration_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(1));
    setup_laser_shot_expiration_timer(*laser_shot_expiration_timer_);

    super_shot_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::milliseconds(100));
    setup_super_shot_timer(*super_shot_timer_);

    laser_shot_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::milliseconds(1));
    setup_laser_shot_timer(*laser_shot_timer_);

    // Timer temporaire de win
    win_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::seconds(12));
    win_timer_->async_wait([this](const boost::system::error_code& ec) {
        if (!ec) {
            med.notify(Sender::GAME, "WIN", {}, MediatorContext());
            //StopAllTimers();
            saveHighScore();
            state = GameState::LOBBY;
            win_timer_->cancel();
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
    game_over_timer_->cancel();
}

void ServerGame::setup_game_over_timer(boost::asio::steady_timer& game_over_timer)
{
    game_over_timer.async_wait([this, &game_over_timer](const boost::system::error_code& ec) {
        if (!ec) {
            if (state == GameState::INGAME && !areAllPlayersDead()) {
                med.notify(Sender::GAME, "LOOSE", {}, MediatorContext());
                saveHighScore();
                //StopAllTimers();
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
    int playerCount = 0;
    int deadPlayerCount = 0;

    for (size_t i = 0; i < types.size(); ++i) {
        if (types[i].has_value() && (types[i].value().type == 5 || types[i].value().type == 30)) {
            playerCount++;
        }
    }
    if (playerCount == 0) {
        std::cout << Colors::RED << "[Game] No players left!" << Colors::RESET << std::endl;
        return false;
    }
    return true;
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
                    } else if (it->type == "decor") {
                        spawnDecor(*it);
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

void ServerGame::setup_laser_shot_timer(boost::asio::steady_timer& laser_shot_timer)
{
    laser_shot_timer.async_wait([this, &laser_shot_timer](const boost::system::error_code& ec) {
        if (!ec) {
            auto& types = reg.get_components<component::type>();
            for(size_t i = 0; i < types.size(); ++i) {
                if (types[i].value().type == 8) {
                    MediatorContext dummyContext;
                    handleDeath(dummyContext, std::vector<std::string>{std::to_string(i)});
                    reg.kill_entity(Entity(i));
                }
            }
            laser_shot_timer.expires_at(laser_shot_timer.expiry() + std::chrono::milliseconds(500));
            setup_laser_shot_timer(laser_shot_timer);
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

void ServerGame::setup_powerup_expiration_timer(boost::asio::steady_timer& invincible_timer)
{
    invincible_timer.async_wait([this, &invincible_timer](const boost::system::error_code& ec) {
        if (!ec) {
            auto& invincibles = reg.get_components<component::invincible>();
            auto& triple_shots = reg.get_components<component::triple_shot>();
            auto& bits = reg.get_components<component::bits>();
            auto& force = reg.get_components<component::force>();
            auto now = std::chrono::steady_clock::now();

            for (size_t i = 0; i < invincibles.size(); ++i) {
                if (invincibles[i].has_value() && invincibles[i].value().is_invincible) {
                    if (now >= invincibles[i].value().expiration_time) {
                        invincibles[i].value().is_invincible = false;
                    }
                }
            }
            for (size_t i = 0; i < triple_shots.size(); ++i) {
                if (triple_shots[i].has_value() && triple_shots[i].value().is_active) {
                    if (now >= triple_shots[i].value().expiration_time) {
                        triple_shots[i].value().is_active = false;
                    }
                }
            }
            for (size_t i = 0; i < force.size(); ++i) {
                if (force[i].has_value() && force[i].value().is_active) {
                    if (now >= force[i].value().expiration_time) {
                        force[i].value().is_active = false;
                    }
                }
            }
            for (size_t i = 0; i < bits.size(); ++i) {
                if (bits[i].has_value() && bits[i].value().is_active) {
                    if (now >= bits[i].value().expiration_time) {
                        bits[i].value().is_active = false;
                    }
                }
            }
            invincible_timer.expires_at(invincible_timer.expiry() + std::chrono::milliseconds(500));
            setup_powerup_expiration_timer(invincible_timer);
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

                    if (types[i].has_value() && types[i].value().type >= 10 && types[i].value().type <= 20) {
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
                        if (types[i].value().type == 10){
                            lua["enemy_ai"](i, positions.x, positions.y);
                        } else if (types[i].value().type == 11){
                            lua["enemy_ai2"](i, positions.x, positions.y);
                        }
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
                auto& types = reg.get_components<component::type>();
                for (size_t i = 0; i < types.size(); ++i) {
                    if (types[i].has_value() && types[i].value().type == 30) {

                        auto const &positions = reg.get_components<component::position>()[i].value();
                        lua["player_ai"](1, positions.x, positions.y); // Appel Lua
                    }
                }

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

void ServerGame::setup_bits_shot_timer(boost::asio::steady_timer& bits_shot_timer)
{
    bits_shot_timer.async_wait([this, &bits_shot_timer](const boost::system::error_code& ec) {
        if (!ec) {
            auto &bits = reg.get_components<component::bits>();
            for (size_t i = 0; i < bits.size(); ++i) {
                if (bits[i].has_value() && bits[i].value().is_active) {
                    Entity bullet = reg.spawn_entity();
                    auto const &positions = reg.get_components<component::position>()[i].value();
                    std::vector<std::string> newParams;
                    newParams.push_back(std::to_string(positions.x));
                    newParams.push_back(std::to_string(positions.y));
                    reg.emplace_component<component::position>(bullet, component::position{positions.x, positions.y});
                    reg.emplace_component<component::velocity>(bullet, component::velocity{5, 0});
                    reg.emplace_component<component::type>(bullet, component::type{9});
                    reg.emplace_component<component::size>(bullet, component::size{10, 10});
                    med.notify(Sender::GAME, "SHOOT", newParams, MediatorContext());
                }
            }
            bits_shot_timer.expires_at(bits_shot_timer.expiry() + std::chrono::milliseconds(10));
            setup_bits_shot_timer(bits_shot_timer);
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
                    reg.emplace_component<component::type>(bullet, component::type{6});
                    reg.emplace_component<component::size>(bullet, component::size{10, 10});
                    med.notify(Sender::GAME, "SHOOT", newParams, MediatorContext());
                }
            }
            force_shot_timer.expires_at(force_shot_timer.expiry() + std::chrono::milliseconds(500));
            setup_force_shot_timer(force_shot_timer);
        }
    });
}

void ServerGame::handleHighScore(const MediatorContext& context, const std::vector<std::string>& params)
{
    std::ifstream file("../src/json/highscore.json");
    if (!file) {
        std::cerr << "Impossible de lire le fichier : ../src/json/highscore.json" << std::endl;
        return;
    }

    nlohmann::json highscoreData;
    file >> highscoreData;
    highscore = highscoreData["highscore"];
    med.notify(Sender::GAME, "HIGHSCORE", {std::to_string(highscore)}, MediatorContext());
    file.close();
    std::cout << "Highscore: " << highscore << std::endl;
}

void ServerGame::saveHighScore()
{
    if (score > highscore) {
        highscore = score;
    }
    nlohmann::json highscoreData;
    highscoreData["highscore"] = highscore;
    std::ofstream file("../src/json/highscore.json");
    file << highscoreData.dump(4);
    file.close();
}

void ServerGame::setup_laser_shot_expiration_timer(boost::asio::steady_timer& laser_shot_timer)
{
    laser_shot_timer.async_wait([this, &laser_shot_timer](const boost::system::error_code& ec) {
        if (!ec) {
            checkLaserExpiration();
            laser_shot_timer.expires_at(laser_shot_timer.expiry() + std::chrono::seconds(1));
            setup_laser_shot_expiration_timer(laser_shot_timer);
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


void ServerGame::setup_super_shot_timer(boost::asio::steady_timer& super_shot_timer)
{
    super_shot_timer.async_wait([this, &super_shot_timer](const boost::system::error_code& ec) {
        if (!ec) {
            auto& super_shots = reg.get_components<component::super_shot>();
            auto now = std::chrono::steady_clock::now();

            for (size_t i = 0; i < super_shots.size(); ++i) {
                if (super_shots[i].has_value() && !super_shots[i].value().is_ready) {
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                        now - super_shots[i].value().last_use_time
                    ).count();
                    
                    if (duration >= component::super_shot::COOLDOWN_SECONDS) {
                        super_shots[i].value().is_ready = true;
                        std::vector<std::string> params = {std::to_string(i)};
                        med.notify(Sender::GAME, "SUPER_SHOT_READY", params, MediatorContext());
                    }
                }
            }
            
            super_shot_timer.expires_at(super_shot_timer.expiry() + std::chrono::milliseconds(100));
            setup_super_shot_timer(super_shot_timer);
        }
    });
}


void ServerGame::spawnPowerUp(JsonEntity entity)
{
    std::cout << "Spawning powerup" << std::endl;
    int x = rand() % 1500 + 300;
    int y = entity.y;
    int type = 0;
    if (entity.subtype == "triple_shoot") {
        type = 0;
    } else if (entity.subtype == "heal") {
        type = 1;
    } else if (entity.subtype == "force") {
        type = 2;
    } else if (entity.subtype == "laser") {
        type = 3;
    } else if (entity.subtype == "bits") {
        type = 4;
    }
    // rajouter d'autres types de powerups ici

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

void ServerGame::spawnDecor(JsonEntity entity)
{
    std::cout << "Spawning decor" << std::endl;
    int x = entity.x;
    int y = entity.y;
    int type = 50;
    if (entity.subtype == "brick") {
        type = 50;
    }
    // rajouter d'autres types de blocs

    Entity powerup = reg.spawn_entity();
    reg.emplace_component<component::position>(powerup, component::position{x, y});
    reg.emplace_component<component::type>(powerup, component::type{type});
    reg.emplace_component<component::size>(powerup, component::size{50, 50});
    reg.emplace_component<component::velocity>(powerup, component::velocity{-5, 0});

    std::vector<std::string> newParams;
    newParams.push_back(std::to_string(type));
    newParams.push_back(std::to_string(x));
    newParams.push_back(std::to_string(y));
    med.notify(Sender::GAME, "POWERUP_SPAWN", newParams);
}


void ServerGame::checkAllCollisions()
{
    auto& positions = reg.get_components<component::position>();
    auto& sizes = reg.get_components<component::size>();
    auto& types = reg.get_components<component::type>();
    auto& healths = reg.get_components<component::health>();
    auto& triple_shots = reg.get_components<component::triple_shot>();
    auto& invincibles = reg.get_components<component::invincible>();
    auto& laser_shots = reg.get_components<component::laser_shot>();
    auto& force = reg.get_components<component::force>();
    auto& bits = reg.get_components<component::bits>();

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
                if ((types[i].value().type == 5 || types[i].value().type == 30) && ((types[j].value().type >= 10 && types[j].value().type <= 13) || types[j].value().type == 50) ) { // MOB vs PLAYER
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
                } else if (((types[i].value().type >= 10 && types[i].value().type <= 13) || types[i].value().type == 50) && (types[j].value().type == 5 || types[j].value().type == 30)) { // MOB vs PLAYER
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
                } else if ((types[i].value().type == 6 || types[i].value().type == 8)  && types[j].value().type >= 10 && types[j].value().type <= 13) { // BULLET vs MOB
                    healths[j].value().hp -= 1000;
                    for (size_t k = 0; k < types.size(); ++k) {
                        if (types[k].has_value() && types[k].value().type == 5) {
                                score += 10;
                                std::vector<std::string> scoreParams = {
                                    std::to_string(k),
                                    std::to_string(score)
                                };
                                med.notify(Sender::GAME, "SCORE_UPDATE", scoreParams, MediatorContext());
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
                } else if (types[i].value().type >= 10 && types[i].value().type <= 13 && (types[j].value().type == 6 || types[i].value().type == 8)) { // BULLET vs MOB
                    healths[i].value().hp -= 1000;
                    for (size_t k = 0; k < types.size(); ++k) {
                        if (types[k].has_value() && types[k].value().type == 5) {
                            score += 10;
                            std::vector<std::string> scoreParams = {
                                std::to_string(k),
                                std::to_string(score)
                            };
                            med.notify(Sender::GAME, "SCORE_UPDATE", scoreParams, MediatorContext());
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
                } else if ((types[i].value().type >= 0 && types[i].value().type <= 4) && types[j].value().type == 5) { // PLAYER vs POWERUP
                    if (types[i].value().type == 0) {
                        triple_shots[j].value().is_active = true;
                        triple_shots[j].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(10);
                    } else if (types[i].value().type == 1) { 
                        healths[j].value().hp += 10;
                    } else if (types[i].value().type == 3) {
                        laser_shots[j].value().is_active = true;
                        laser_shots[j].value().activation_time = std::chrono::steady_clock::now();
                    } else if (types[i].value().type == 2) {
                        //Caractéristiques principales :
                        // tirer en avant, en arriere, bloque les balles ennemies
                        invincibles[j].value().is_invincible = true;
                        invincibles[j].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(4);
                        force[j].value().is_active = true;
                        force[j].value().is_front = 0;
                        force[j].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(4);
                    } else if (types[i].value().type == 4) {
                        bits[j].value().is_active = true;
                        bits[j].value().is_up = 0;
                        bits[j].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(10);
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
                } else if ((types[j].value().type >= 0 && types[j].value().type <= 4) && types[i].value().type == 5) { // PLAYER vs POWERUP
                    if (types[j].value().type == 0) {
                        triple_shots[i].value().is_active = true;
                        triple_shots[i].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(10);
                    } else if (types[j].value().type == 1) {
                        healths[i].value().hp += 10;
                    } else if (types[j].value().type == 3) {
                        laser_shots[i].value().is_active = true;
                        laser_shots[i].value().activation_time = std::chrono::steady_clock::now();
                    } else if (types[j].value().type == 2) {
                        invincibles[i].value().is_invincible = true;
                        invincibles[i].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(4);
                        force[i].value().is_active = true;
                        force[i].value().is_front = 0;
                        force[i].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(4);
                    } else if (types[j].value().type == 4) {
                        bits[i].value().is_active = true;
                        bits[i].value().is_up = 0;
                        bits[i].value().expiration_time = std::chrono::steady_clock::now() + std::chrono::seconds(10);
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



void ServerGame::checkLaserExpiration()
{
    auto& laser_shots = reg.get_components<component::laser_shot>();
    auto now = std::chrono::steady_clock::now();

    for (size_t i = 0; i < laser_shots.size(); ++i) {
        if (laser_shots[i] && laser_shots[i].value().is_active) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                now - laser_shots[i].value().activation_time
            ).count();

            if (duration >= 5) {
                // Désactiver le power-up
                laser_shots[i].value().is_active = false;
            }
        }
    }
}


void ServerGame::addEntityToLevel(int entityType, int x, int y, std::string filename)
{
    nlohmann::json entityData;

    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier level1.json." << std::endl;
        return;
    }

    nlohmann::json levelData;
    inputFile >> levelData;
    inputFile.close();

    if (entityType == 0) {
        entityData = {
            {"type", "powerup"},
            {"subtype", "heal"},
            {"position", { {"x", x}, {"y", y} }}
        };
    } else if (entityType == 1) {
        entityData = {
            {"type", "powerup"},
            {"subtype", "triple_shoot"},
            {"position", { {"x", x}, {"y", y} }}
        };
    } else if (entityType == 2) {
        entityData = {
            {"type", "powerup"},
            {"subtype", "force"},
            {"position", { {"x", x}, {"y", y} }}
        };
    } else if (entityType == 3) {
        entityData = {
            {"type", "powerup"},
            {"subtype", "laser"},
            {"position", { {"x", x}, {"y", y} }}
        };
    } else if (entityType == 10) {
        entityData = {
            {"type", "mob"},
            {"subtype", "enemy1"},
            {"position", { {"x", x}, {"y", y} }}
        };
    } else if (entityType == 11) {
        entityData = {
            {"type", "mob"},
            {"subtype", "enemy2"},
            {"position", { {"x", x}, {"y", y} }}
        };
    } else if (entityType == 50) {
        entityData = {
        {"type", "decor"},
        {"subtype", "brick"},
        {"position", { {"x", x}, {"y", y} }}
      };
    }

    if (levelData.contains("entities") && levelData["entities"].is_array()) {
        levelData["entities"].push_back(entityData);
    } else {
        std::cerr << "Erreur : Le fichier JSON ne contient pas une liste d'entités valide." << std::endl;
        return;
    }
    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier level1.json en écriture." << std::endl;
        return;
    }

    outputFile << levelData.dump(4); // Écrit le JSON avec une indentation de 4 espaces
    outputFile.close();
}





//===================================COMMANDS=================================

void ServerGame::handleLevelEditor(const MediatorContext& context, const std::vector<std::string>& params)
{
    if (params.size() == 0){
        std::string directoryPath = "../src/json";
        // Vecteur pour stocker les fichiers valides
        std::vector<std::string> fileList;
        int maxLevel = 0;

        try {
            // Parcours des fichiers dans le dossier
            for (const auto& entry : fs::directory_iterator(directoryPath)) {
                if (entry.is_regular_file()) {
                    std::string fileName = entry.path().filename().string();
                    std::cout << "Fichier : " << fileName << std::endl;

                    // Vérifier si le fichier suit le format "levelX.json" avec une regex
                    std::regex levelRegex(R"(level(\d+)\.json)");
                    std::smatch match;

                    if (std::regex_match(fileName, match, levelRegex)) {
                        fileList.push_back(fileName);

                        // Extraire le numéro de niveau et mettre à jour maxLevel
                        int level = std::stoi(match[1].str());
                        maxLevel = std::max(maxLevel, level);
                    }
                }
            }
            med.notify(Sender::GAME, "LEVEL_EDITOR", fileList, context);
        } catch (const std::exception& e) {
            std::cerr << "Erreur : " << e.what() << std::endl;
        }
    }
    if (params.size() < 4) {
        std::cerr << "Erreur : Pas assez de paramètres pour la commande CREATE_ENTITY." << std::endl;
        return;
    }

    int entityType = std::stoi(params[0]);
    int x = std::stoi(params[1]);
    int y = std::stoi(params[2]);
    std::string filename = params[3];
    addEntityToLevel(entityType, x, y, filename);
}

void ServerGame::handleConnect(const MediatorContext& context, const std::vector<std::string>& params)
{
    handleHighScore(context, params);
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
    reg.emplace_component<component::laser_shot>(player, component::laser_shot{false, {}});
    reg.emplace_component<component::super_shot>(player, component::super_shot{true, {}});
    reg.emplace_component<component::score>(player, component::score{0});
    reg.emplace_component<component::force>(player, component::force{false, {}});
    reg.emplace_component<component::bits>(player, component::bits{false, {}});

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
        reg.emplace_component<component::type>(Entity(1), component::type{30}); //AI type
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
        auto& laser_shots = reg.get_components<component::laser_shot>();
        if(laser_shots.size() > player_id && laser_shots[player_id].value().is_active){
            handleLaserShoot(context, params);
            return;
        } else if (triple_shots.size() > player_id && triple_shots[player_id].value().is_active) {
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
                // handleMoove()
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
            //handleMoove(
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

void ServerGame::handleLaserShoot(const MediatorContext& context, const std::vector<std::string>& params)
{
    if (state == GameState::LOBBY) {
        return;
    }
    try {
        params.size() == 0 ? throw std::runtime_error("No params") : 0;
        int player_id = std::stoi(params[0]);
        auto const &positions = reg.get_components<component::position>()[std::stoi(params[0])].value();
        auto& laser_shots = reg.get_components<component::laser_shot>();

        if (laser_shots.size() > player_id && laser_shots[player_id].value().is_active) {
            Entity bullet = reg.spawn_entity();
            std::vector<std::string> newParams;
            newParams.push_back(std::to_string(positions.x));
            newParams.push_back(std::to_string(positions.y));

            reg.emplace_component<component::position>(bullet, component::position{positions.x, positions.y});
            reg.emplace_component<component::velocity>(bullet, component::velocity{0, 0});
            reg.emplace_component<component::type>(bullet, component::type{8});
            reg.emplace_component<component::size>(bullet, component::size{1900, 10}); // Fine mais longue

            med.notify(Sender::GAME, "LASER_SHOOT", newParams, context);
        }
    } catch(const std::exception& e) {
        std::cout << Colors::RED << "[Error] Exception in handleLaserShoot: " << e.what() << Colors::RESET << std::endl;
        return;
    }
}

void ServerGame::handleSuperShoot(const MediatorContext& context, const std::vector<std::string>& params)
{
    if (state == GameState::LOBBY) {
        return;
    }
    try {
        params.size() == 0 ? throw std::runtime_error("No params") : 0;
        int player_id = std::stoi(params[0]);
        auto const &positions = reg.get_components<component::position>()[std::stoi(params[0])].value();
        auto& super_shots = reg.get_components<component::super_shot>();

        if (super_shots.size() > player_id && super_shots[player_id].value().is_ready) {
            // Super tir (plus grand et plus puissant)
            Entity bullet = reg.spawn_entity();
            std::vector<std::string> newParams;
            newParams.push_back(std::to_string(positions.x));
            newParams.push_back(std::to_string(positions.y));

            reg.emplace_component<component::position>(bullet, component::position{positions.x, positions.y});
            reg.emplace_component<component::velocity>(bullet, component::velocity{7, 0});
            reg.emplace_component<component::type>(bullet, component::type{6}); // Type pour super tir
            reg.emplace_component<component::size>(bullet, component::size{40, 40}); // Plus grand

            // Mettre à jour le cooldown
            super_shots[player_id].value().is_ready = false;
            super_shots[player_id].value().last_use_time = std::chrono::steady_clock::now();

            med.notify(Sender::GAME, "SUPER_SHOOT", newParams, context);
        }
    } catch(const std::exception& e) {
        std::cout << Colors::RED << "[Error] Exception in handleSuperShoot: " << e.what() << Colors::RESET << std::endl;
        return;
    }
}
