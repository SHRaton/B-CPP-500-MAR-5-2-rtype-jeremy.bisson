#include "Core.hpp"
#include "../Systems.hpp"
#include "../server/ServerNetwork.hpp"

std::string Core::encode_action(GameAction action)
{
    std::bitset<6> bits(static_cast<unsigned long>(action));
    return bits.to_string();
}

void Core::setup_position_timer(boost::asio::steady_timer& position_timer)
{
    position_timer.async_wait([this, &position_timer](const boost::system::error_code& ec) {
        if (!ec) {
            Systems::position_system(reg);
            position_timer.expires_at(position_timer.expiry() + std::chrono::milliseconds(10));
            setup_position_timer(position_timer);
        }
    });
}

void Core::checkInvincibility()
{
    auto& invincibles = reg.get_components<component::invincible>();
    auto& drawables = reg.get_components<component::drawable>();
    auto now = std::chrono::steady_clock::now();

    for (size_t i = 0; i < invincibles.size(); ++i) {
        if (invincibles[i].value().is_invincible && now >= invincibles[i].value().expiration_time) {
            invincibles[i].value().is_invincible = false;
            drawables[i].value().sprite.setColor(sf::Color::White);
        }
    }
}

void Core::displayRegistryInfo()
{
    auto& positions = reg.get_components<component::position>();
    auto& velocities = reg.get_components<component::velocity>();
    auto& healths = reg.get_components<component::health>();
    auto& drawables = reg.get_components<component::drawable>();
    auto& controllables = reg.get_components<component::controllable>();
    auto& invincibles = reg.get_components<component::invincible>();
    auto& damages = reg.get_components<component::damage>();
    auto& types = reg.get_components<component::type>();

    float yPosition = 50.0f; // Position initiale en Y pour le texte

    for (size_t i = 0; i < positions.size(); ++i) {
        sf::Color entityColor = sf::Color::White; // Couleur par défaut

        std::ostringstream oss;
        oss << "Entity " << i;
        if (types.size() > i && types[i]) {
            switch (types[i].value().type) {
                case 667: // Vaisseau
                    oss << "( Other SpaceShip ) :\n";
                    entityColor = sf::Color::Green;
                    break;
                case 696: // Vaisseau
                    oss << "( My SpaceShip ) :\n";
                    entityColor = sf::Color::Blue;
                    break;
                case 10: // Mob
                    oss << "( Mob 1 ) :\n";
                    entityColor = sf::Color::Red;
                    break;
                case 11: // Mob
                    oss << "( Mob 2 ) :\n";
                    entityColor = sf::Color::Red;
                    break;
                case 0: // PowerUp
                    oss << "( PowerUp 1 ) :\n";
                    entityColor = sf::Color::White;
                    break;
                case 1: // PowerUp
                    oss << "( PowerUp 2 ) :\n";
                    entityColor = sf::Color::White;
                    break;
                case 6: // Missile
                    oss << "( Bullet ) :\n";
                    entityColor = sf::Color::Yellow;
                    break;
                case 7: // Mob Bullet
                    oss << "( Mob Bullet ) :\n";
                    entityColor = sf::Color::Magenta;
                    break;
                default:
                    oss << "( Unknown ):\n";
                    entityColor = sf::Color::White;
                    break;
            }
        }

        if (positions[i]) {
            oss << "  Position: (" << positions[i].value().x << ", " << positions[i].value().y << ")  |  ";
        }
        if (velocities.size() > i && velocities[i]) {
            oss << "  Velocity: (" << velocities[i].value().vx << ", " << velocities[i].value().vy << ")  |  ";
        }
        if (healths.size() > i && healths[i]) {
            oss << "  Health: " << healths[i].value().hp << "  |  ";
        }
        if (drawables.size() > i && drawables[i]) {
            oss << "  Drawable: Yes  |  ";
        }
        if (controllables.size() > i && controllables[i]) {
            oss << "  Controllable: " << (controllables[i].value().is_controllable ? "Yes" : "No") << "  |  ";
        }
        if (invincibles.size() > i && invincibles[i]) {
            oss << "  Invincible: " << (invincibles[i].value().is_invincible ? "Yes" : "No") << "  |  ";
        }
        if (damages.size() > i && damages[i]) {
            oss << "  Damage: " << damages[i].value().dmg << "  |  ";
        }
        oss << "\n";

        // Créer un texte SFML pour cette entité
        sf::Text entityInfo;
        entityInfo.setFont(font);
        entityInfo.setCharacterSize(20);
        entityInfo.setFillColor(sf::Color::Black);
        entityInfo.setPosition(10, yPosition);
        entityInfo.setString(oss.str());

        float textHeight = entityInfo.getLocalBounds().height + 10.0f;
        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(registryWindow.getSize().x, textHeight));
        rect.setFillColor(entityColor);
        rect.setPosition(10, yPosition);


        // Dessiner le texte dans la deuxième fenêtre
        registryWindow.draw(rect);
        registryWindow.draw(entityInfo);


        // Mettre à jour la position Y pour le prochain texte
        yPosition += textHeight;
    }
    sf::Text nbPlayer;
    nbPlayer.setFont(font);
    nbPlayer.setCharacterSize(25);
    nbPlayer.setFillColor(sf::Color::White);
    nbPlayer.setPosition(10, 10);
    nbPlayer.setString("Number of player : " + std::to_string(nb_player));
    registryWindow.draw(nbPlayer);

    sf::Text myId;
    myId.setFont(font);
    myId.setCharacterSize(25);
    myId.setFillColor(sf::Color::White);
    myId.setPosition(400, 10);
    myId.setString("My ID : " + std::to_string(network->getId()));
    registryWindow.draw(myId);
}


void Core::update_hud()
{
    fpsText.setFont(font);
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::Green);
    fpsText.setPosition(15, 10);
    
    latencyText.setFont(font);
    latencyText.setCharacterSize(20);
    latencyText.setFillColor(sf::Color::Green);
    latencyText.setPosition(15, 35);

    float fps = 1.f / fpsClock.restart().asSeconds();
    if (latencyClock.getElapsedTime().asSeconds() > 1.f) {
        int latency = 32;
        latencyText.setString("Ping: " + std::to_string(latency) + " ms");
        fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
        latencyClock.restart();
    }
    if (scoreScale > 1.0f) {
        float deltaTime = scoreAnimClock.restart().asSeconds();
        scoreScale -= deltaTime * 2.0f;
        if (scoreScale < 1.0f) {
            scoreScale = 1.0f;
            globalScore_text.setFillColor(sf::Color::White);
        }
        globalScore_text.setScale(scoreScale, scoreScale);
    }

    if (isScrollingBackground) {
        sf::Time elapsed = clock_scrolling.restart();
        float deltaTime = elapsed.asSeconds();
        currentMap.getBackgroundSprite().move(-4.5 * 60 * deltaTime, 0);
    }

    updateAnimations();
    float deltaTime = clock_explosion.restart().asSeconds();
    updateExplosions(deltaTime);
}

void Core::gui_gamewin() {
    if (!gameWinTexture.loadFromFile("../ressources/sprites/win.png")) {
        throw std::runtime_error("Error loading game win texture");
    }
    gameWinSprite.setTexture(gameWinTexture);
    Game1Music.stop();
    Game2Music.stop();
    Game3Music.stop();
    Game4Music.stop();
    winMusic.play();
    
    // Centrer l'image
    gameWinSprite.setPosition(
        window.getSize().x / 2 - gameWinSprite.getGlobalBounds().width / 2,
        window.getSize().y / 2 - gameWinSprite.getGlobalBounds().height / 2
    );

    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::DISCONNECT) << " " << network->getId();
                network->send(messageStream.str());
                window.close();
                exit(0);
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
                if (save_replay.getGlobalBounds().contains(worldPos)) {
                    buttonSound_click.play();
                    std::ostringstream messageStream;
                    messageStream << encode_action(GameAction::SAVE_REPLAY);
                    network->send(messageStream.str());
                }
            }
        }

        window.clear(sf::Color::Black);
        window.draw(gameWinSprite);
        window.draw(save_replay);
        window.display();
    }
}


void Core::gui_gameover() {
    if (!gameOverTexture.loadFromFile("../ressources/sprites/lose.png")) {
        throw std::runtime_error("Error loading game over texture");
    }
    gameOverSprite.setTexture(gameOverTexture);
    
    // Arrêt et démarrage des musiques avec fade out
    const float musicFadeSpeed = 2.0f;
    float currentVolume = 100.0f;
    while (currentVolume > 0) {
        currentVolume -= musicFadeSpeed;
        Game1Music.setVolume(currentVolume);
        BossMusic1.setVolume(currentVolume);
        sf::sleep(sf::milliseconds(10));
    }
    Game1Music.stop();
    Game2Music.stop();
    Game3Music.stop();
    Game4Music.stop();
    BossMusic1.stop();
    
    // Démarrage de la musique game over avec fade in
    gameoverMusic.setVolume(0);
    gameoverMusic.play();
    float gameoverVolume = 0;
    while (gameoverVolume < 100) {
        gameoverVolume += musicFadeSpeed;
        gameoverMusic.setVolume(gameoverVolume);
        sf::sleep(sf::milliseconds(10));
    }

    // Configuration des effets visuels
    float fadeAlpha = 0.0f;
    const float fadeSpeed = 255.0f;
    float scale = 1.2f;
    const float scaleSpeed = 0.3f;
    const float shakeIntensity = 5.0f;
    
    sf::Clock effectsClock;
    sf::Clock shakeClock;
    bool effectsComplete = false;
    
    // Position centrale initiale
    sf::Vector2f centerPosition(
        window.getSize().x / 2 - gameOverSprite.getGlobalBounds().width / 2,
        window.getSize().y / 2 - gameOverSprite.getGlobalBounds().height / 2
    );
    gameOverSprite.setPosition(centerPosition);
    gameOverSprite.setOrigin(
        gameOverSprite.getGlobalBounds().width / 2,
        gameOverSprite.getGlobalBounds().height / 2
    );
    gameOverSprite.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    // Système de particules amélioré
    struct Particle {
        sf::RectangleShape shape;
        sf::Vector2f velocity;
        float lifetime;
        float maxLifetime;
        float size;
    };
    std::vector<Particle> particles;
    sf::Clock particleClock;

    sf::Event event;
    while (window.isOpen()) {
        float deltaTime = effectsClock.restart().asSeconds();

        // Gestion des événements
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)) {
                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::DISCONNECT) << " " << network->getId();
                network->send(messageStream.str());
                window.close();
                exit(0);
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
                if (save_replay.getGlobalBounds().contains(worldPos)) {
                    buttonSound_click.play();
                    std::ostringstream messageStream;
                    messageStream << encode_action(GameAction::SAVE_REPLAY);
                    network->send(messageStream.str());
                }
            }
        }
        if (!effectsComplete) {
            // Fade in
            if (fadeAlpha < 255.0f) {
                fadeAlpha = std::min(fadeAlpha + (fadeSpeed * deltaTime), 255.0f);
                gameOverSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(fadeAlpha)));
            }
            if (scale > 1.0f) {
                scale = std::max(1.0f, scale - (scaleSpeed * deltaTime));
                gameOverSprite.setScale(scale, scale);
            }
            if (shakeClock.getElapsedTime().asSeconds() < 0.5f) {
                float shakeX = (std::rand() % 100 - 50) * 0.01f * shakeIntensity;
                float shakeY = (std::rand() % 100 - 50) * 0.01f * shakeIntensity;
                gameOverSprite.setPosition(
                    window.getSize().x / 2 + shakeX,
                    window.getSize().y / 2 + shakeY
                );
            }
            if (fadeAlpha >= 255.0f && scale <= 1.0f) {
                effectsComplete = true;
                gameOverSprite.setPosition(window.getSize().x / 2, window.getSize().y / 2);
            }
        }
        if (particleClock.getElapsedTime().asSeconds() > 0.01f) {
        for (int i = 0; i < 8; i++) { // Augmentation du nombre de particules
            Particle particle;
            particle.size = 3.0f + (std::rand() % 5);
            particle.shape.setSize(sf::Vector2f(particle.size, particle.size));
            int r = 255;
            int g = std::rand() % 100 + 50;
            particle.shape.setFillColor(sf::Color(r, g, 0, 255));
            particle.shape.setPosition(
                std::rand() % window.getSize().x,  // Position X aléatoire sur toute la largeur
                window.getSize().y + 10
            );
            float centerDistance = std::abs((window.getSize().x / 2) - particle.shape.getPosition().x);
            float velocityMultiplier = 1.0f - (centerDistance / (window.getSize().x / 2)) * 0.3f;
            
            particle.velocity = sf::Vector2f(
                (std::rand() % 200 - 100) * 0.5f,
                (-250 - (std::rand() % 150)) * velocityMultiplier // Vitesse verticale variable
            );
            particle.maxLifetime = 1.5f + (std::rand() % 100) / 100.0f;
            particle.lifetime = particle.maxLifetime;
            particles.push_back(particle);
        }
        particleClock.restart();
    }

    // Mise à jour des particules
    for (auto it = particles.begin(); it != particles.end();) {
        it->lifetime -= deltaTime;
        it->shape.move(it->velocity * deltaTime);
        float centerOffset = (window.getSize().x / 2) - it->shape.getPosition().x;
        it->velocity.x += centerOffset * 0.02f * deltaTime; // Légère attraction vers le centre
        it->velocity.y += 50.f * deltaTime; // Gravité réduite
        float scale = it->lifetime / it->maxLifetime;
        it->shape.setSize(sf::Vector2f(it->size * scale, it->size * scale));
        float alpha = (it->lifetime / it->maxLifetime) * 255;
        sf::Color currentColor = it->shape.getFillColor();
        it->shape.setFillColor(sf::Color(currentColor.r, currentColor.g, currentColor.b, static_cast<sf::Uint8>(alpha)));
        if (it->lifetime <= 0) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
        window.clear(sf::Color::Black);
        window.draw(gameOverSprite);
        window.draw(save_replay);
        for (const auto& particle : particles) {
            window.draw(particle.shape);
        }
        window.display();
    }
}

void Core::draw_hitbox()
{
    if (!isHitbox) {
        return;
    }
    auto const &positions = reg.get_components<component ::position>();
    auto const &drawables = reg.get_components<component ::drawable>();
    for (size_t i = 0; i < positions.size() && i < drawables.size(); ++i)
    {
        auto const &pos = positions[i];
        auto const &drawable = drawables[i];
        if (pos && drawable)
        {
            sf::RectangleShape hitbox(sf::Vector2f(
                drawable.value().sprite.getGlobalBounds().width,
                drawable.value().sprite.getGlobalBounds().height
            ));
            hitbox.setFillColor(sf::Color::Transparent);
            hitbox.setOutlineColor(sf::Color::Red);
            hitbox.setOutlineThickness(5);
            hitbox.setPosition(pos.value().x, pos.value().y);
            renderTexture.draw(hitbox);
        }
    }
}

void Core::display_all()
{
    window.clear();
    renderTexture.clear(sf::Color::Black);
    if (isDead) {
        states.shader = &blackAndWhiteShader;
    } else {
        states.shader = nullptr;
    }
    renderTexture.draw(currentMap.getBackgroundSprite());
    renderTexture.draw(PlayerHUD1);
    renderTexture.draw(PlayerHUD2);
    renderTexture.draw(PlayerHUD3);
    renderTexture.draw(PlayerHUD4);
    renderTexture.draw(hudBackground);
    renderTexture.draw(scoreBackground);
    sys.draw_system(reg, renderTexture);
    draw_hitbox();
    renderTexture.draw(fpsText);
    renderTexture.draw(latencyText);
    renderTexture.draw(globalScore_text);
    if (!isDead) {
        renderTexture.draw(shootBar);
        renderTexture.draw(superShootBar);
        if (laserActive) {
            if (laserClock.getElapsedTime().asSeconds() <= 4.95) {
                renderTexture.draw(laserPowerUpLogo);
            } else {
                laserActive = false;
            }
        }
    }
    for (const auto& explosion : activeExplosions) {
        if (explosion.isActive) {
            renderTexture.draw(explosion.sprite);
        }
    }
    for (const auto& player : otherPlayers) {
        renderTexture.draw(player.hpText);
    }
    if (isDead) {
        sf::Text deadText;
        deadText.setFont(font);
        deadText.setCharacterSize(50);
        deadText.setFillColor(sf::Color::Red);
        deadText.setPosition(window.getSize().x - 450, window.getSize().y - 80);
        deadText.setString("You are Dead");
        renderTexture.draw(deadText);
    }
    renderTexture.display();
    sf::Sprite screenSprite(renderTexture.getTexture());
    if (daltonismType != DaltonismType::NONE && !isDead) {
        window.draw(screenSprite, &daltonismShader);
    } else if (isDead) {
        window.draw(screenSprite, states);
    } else {
        window.draw(screenSprite);
    }

    if (isFlashing) {
        float elapsedTime = flashClock.getElapsedTime().asSeconds();
        if (elapsedTime <= 2.0f) {
            // Réduit l'intensité maximale à 100 au lieu de 255
            flashAlpha = 100.0f * std::abs(std::sin(elapsedTime * 3.14159f * 1.5f)); // Réduit la vitesse de pulsation
            flashOverlay.setFillColor(sf::Color(255, 0, 0, static_cast<sf::Uint8>(flashAlpha)));
            window.draw(flashOverlay);
        } else {
            isFlashing = false;
        }
    }

    window.display();
    registryWindow.clear();
    displayRegistryInfo();
    registryWindow.display();
}

void Core::gui_game()
{
    loadAssetsGame();
    save_replay.setScale(1, 1);
    save_replay.setPosition(1600, 25);
    sf::Event event;

    menuMusic.stop();
    if (levelSelected == 1) {
        Game1Music.play();
    } else if (levelSelected == 2) {
        Game2Music.play();
    } else if (levelSelected == 3) {
        Game3Music.play();
    } else if (levelSelected == 4) {
        Game4Music.play();
    }
    registryWindow.create(sf::VideoMode(1400, 1080), "ECS Debuger");

    position_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_, std::chrono::milliseconds(2));
    setup_position_timer(*position_timer_);
    io_thread_ = std::thread([this]() {
            io_context_.run();
    });

    if (daltonismType != DaltonismType::NONE) {
        if (!loadDaltonismShader(daltonismShader, daltonismType)) {
            throw std::runtime_error("Error when loading Shader for daltonism");
        }
    }
    if (!loadBlackAndWhiteShader()) {
        throw std::runtime_error("Error when loading black and white shader");
    }
    //playIntroAnimation();
    while (window.isOpen() && registryWindow.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::DISCONNECT) << " " << network->getId();
                network->send(messageStream.str());
                window.close();
                exit (0);
            }
            if (event.type == sf::Event::KeyPressed) {
                keysPressed[event.key.code] = true;
            }
            if (event.type == sf::Event::KeyReleased) {
                keysPressed[event.key.code] = false;
            }
        }
        while (registryWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                std::ostringstream messageStream;
                messageStream << encode_action(GameAction::DISCONNECT) << " " << network->getId();
                network->send(messageStream.str());
                window.close();
                registryWindow.close();
                exit (0);
            }
        }

        if (currentMap.getBackgroundSprite().getPosition().x <= -14190) {
            gui_gamewin();
            return;
        }

        handleServerCommands();
        update_hud();
        control_system();
        checkInvincibility();
        display_all();
    }
}