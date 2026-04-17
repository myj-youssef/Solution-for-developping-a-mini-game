#include "../include/Game.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

namespace {
    bool loadTextureWithMask(sf::Texture& tex, const std::string& filepath) {
        sf::Image img;
        if (!img.loadFromFile(filepath)) return false;
        img.createMaskFromColor(sf::Color::White);
        return tex.loadFromImage(img);
    }
}

Game::Game() : window(sf::VideoMode(800, 600), "Application de Jeu de Cachette"), state(GameState::Menu) {
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    loadAssets();

    ground.setSize(sf::Vector2f(800.f, 100.f));
    ground.setFillColor(sf::Color(100, 100, 100)); // Gris opaque par défaut
    ground.setPosition(0.f, 500.f);
    
    if (hasGroundTex) {
        groundTex.setRepeated(true);
        ground.setTexture(&groundTex);
        ground.setTextureRect(sf::IntRect(0, 0, 800, 100));
        ground.setFillColor(sf::Color::White); // Réinitialise la teinte pour la texture
    }

    timeLimit = 60.f;
    
    initUI();
    resetGame();
}

/**
 * @brief Chargement centralisé des fichiers.
 * Évite de lire le même PNG sur le disque dur à chaque spawn de Mine ou de Drone (Optimisation).
 */
void Game::loadAssets() {
    fontLoaded = font.loadFromFile("assets/arial.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("arial.ttf");

    hasBgTexture = bgTexture.loadFromFile("assets/background.png");
    if (hasBgTexture) {
        bgSprite1.setTexture(bgTexture);
        bgSprite2.setTexture(bgTexture);
        float scaleX = 800.f / bgTexture.getSize().x;
        float scaleY = 600.f / bgTexture.getSize().y;
        bgSprite1.setScale(scaleX, scaleY);
        bgSprite2.setScale(scaleX, scaleY);
        bgSprite1.setPosition(0.f, 0.f);
        bgSprite2.setPosition(800.f, 0.f);
    } else {
        std::cerr << "AVERTISSEMENT: 'assets/background.png' absent." << std::endl;
    }

    // Chargement unique des Textures d'entités avec masque transparent
    hasPRun1 = loadTextureWithMask(playerTexRun1, "assets/player.png");
    hasPRun2 = loadTextureWithMask(playerTexRun2, "assets/player2.png");
    hasPDuck = loadTextureWithMask(playerTexDuck, "assets/player_duck.png");
    hasEnemyTex = loadTextureWithMask(enemyTex, "assets/enemy.png");
    hasMineTex = loadTextureWithMask(mineTex, "assets/mine.png");
    hasDroneTex = loadTextureWithMask(droneTex, "assets/drone.png");
    hasBirdTex = loadTextureWithMask(birdTex, "assets/bird.png");
    hasTreasureTex = loadTextureWithMask(treasureTex, "assets/treasure.png");
    hasPlatformTex = loadTextureWithMask(platformTex, "assets/platform.png");
    hasCollectibleTex = loadTextureWithMask(collectibleTex, "assets/banana.png");
    
    hasGroundTex = groundTex.loadFromFile("assets/ground.png");
}

Game::~Game() {
    for (size_t i = 0; i < obstacles.size(); ++i) delete obstacles[i];
    obstacles.clear();
    for (size_t i = 0; i < platforms.size(); ++i) delete platforms[i];
    platforms.clear();
    for (size_t i = 0; i < collectibles.size(); ++i) delete collectibles[i];
    collectibles.clear();
}

void Game::initUI() {
    if (fontLoaded) {
        titleText.setFont(font);
        instructionText.setFont(font);
        statusText.setFont(font);
        scoreText.setFont(font);
    }
    
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition(20.f, 20.f);

    titleText.setCharacterSize(50);
    titleText.setFillColor(sf::Color::White);
    titleText.setString("Jeu de Cachette (Bunker)");
    
    sf::FloatRect textRect = titleText.getLocalBounds();
    titleText.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
    titleText.setPosition(sf::Vector2f(400.0f, 150.0f));

    instructionText.setCharacterSize(24);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setString("Appuyez sur [Entree] pour Jouer\n[A] pour A Propos\n[Echap] pour Quitter");
    
    textRect = instructionText.getLocalBounds();
    instructionText.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
    instructionText.setPosition(sf::Vector2f(400.0f, 300.0f));

    statusText.setCharacterSize(40);
    
    progressBarBackground.setSize(sf::Vector2f(400.f, 20.f));
    progressBarBackground.setFillColor(sf::Color(50, 50, 50, 200));
    progressBarBackground.setPosition(200.f, 20.f);
    
    progressBarFill.setSize(sf::Vector2f(0.f, 20.f));
    progressBarFill.setFillColor(sf::Color::Cyan);
    progressBarFill.setPosition(200.f, 20.f);

    healthBarFill.setSize(sf::Vector2f(200.f, 20.f));
    healthBarFill.setFillColor(sf::Color::Red);
    healthBarFill.setPosition(650.f, 20.f); // Mettre en haut a droite
    
    healthText.setString("Sang");
    if (fontLoaded) healthText.setFont(font);
    healthText.setCharacterSize(18);
    healthText.setFillColor(sf::Color::White);
    healthText.setPosition(650.f, 0.f);
}

void Game::resetGame() {
    elapsedTime = 0.f;
    obstacleSpawnTimer = 0.f;
    bananaScore = 0;
    treasureSpawned = false;
    
    for (size_t i = 0; i < obstacles.size(); ++i) delete obstacles[i];
    obstacles.clear();
    for (size_t i = 0; i < platforms.size(); ++i) delete platforms[i];
    platforms.clear();
    for (size_t i = 0; i < collectibles.size(); ++i) delete collectibles[i];
    collectibles.clear();
    
    // L'objet Player est remplacé, et reçoit les Textures paramétrées via références (const sf::Texture&)
    player = Player(playerTexRun1, hasPRun1, playerTexRun2, hasPRun2, playerTexDuck, hasPDuck);
}

void Game::spawnEntities() {
    if (elapsedTime >= timeLimit - 5.f) {
        if (!treasureSpawned) {
            obstacles.push_back(new Treasure(400.f, 850.f, treasureTex, hasTreasureTex));
            treasureSpawned = true;
        }
        return;
    }
    
    int type = std::rand() % 3;
    float startX = 850.f;
    
    if (type == 0) {
        int obsType = std::rand() % 4;
        if (obsType == 0) obstacles.push_back(new Enemy(500.f, startX, enemyTex, hasEnemyTex));
        else if (obsType == 1) obstacles.push_back(new Mine(500.f, startX, mineTex, hasMineTex));
        else if (obsType == 2) obstacles.push_back(new Drone(350.f + (std::rand()%100), startX, droneTex, hasDroneTex));
        else if (obsType == 3) obstacles.push_back(new Bird(200.f + (std::rand()%150), startX, birdTex, hasBirdTex));
    } else if (type == 1) {
        // Spawn Platform and Collectible
        float pHeight = 350.f + (std::rand() % 80);
        platforms.push_back(new Platform(pHeight, startX, 150.f, platformTex, hasPlatformTex));
        if (std::rand() % 2 == 0) {
            collectibles.push_back(new Collectible(pHeight - 40.f, startX + 50.f, collectibleTex, hasCollectibleTex));
        }
    } else {
        // Spawn Collectible without platform
        collectibles.push_back(new Collectible(350.f + (std::rand() % 100), startX, collectibleTex, hasCollectibleTex));
    }
}

void Game::run() {
    sf::Clock dtClock;
    while (window.isOpen()) {
        float deltaTime = dtClock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) window.close();

            if (state == GameState::Menu) {
                if (event.key.code == sf::Keyboard::Enter) {
                    resetGame();
                    state = GameState::Playing;
                } else if (event.key.code == sf::Keyboard::A) {
                    state = GameState::About;
                }
            } else if (state == GameState::About) {
                if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Escape) state = GameState::Menu;
            } else if (state == GameState::GameOver || state == GameState::Win) {
                if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::R) state = GameState::Menu;
            } else if (state == GameState::Playing) {
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Space) {
                    player.jump();
                } else if (event.key.code == sf::Keyboard::Down) {
                    player.duck();
                }
            }
        }
        
        if (event.type == sf::Event::KeyReleased) {
            if (state == GameState::Playing && event.key.code == sf::Keyboard::Down) player.standUp();
        }
    }
}

void Game::update(float deltaTime) {
    if (state != GameState::Playing) return;

    if (hasBgTexture) {
        float bgSpeed = 100.f; 
        bgSprite1.move(-bgSpeed * deltaTime, 0);
        bgSprite2.move(-bgSpeed * deltaTime, 0);

        if (bgSprite1.getPosition().x + 800.f <= 0) bgSprite1.setPosition(bgSprite2.getPosition().x + 800.f, 0);
        if (bgSprite2.getPosition().x + 800.f <= 0) bgSprite2.setPosition(bgSprite1.getPosition().x + 800.f, 0);
    }

    elapsedTime += deltaTime;
    obstacleSpawnTimer += deltaTime;

    float progressRatio = elapsedTime / timeLimit;
    if (progressRatio > 1.f) progressRatio = 1.f;
    progressBarFill.setSize(sf::Vector2f(400.f * progressRatio, 20.f));

    scoreText.setString("Bananes : " + std::to_string(bananaScore));

    if (elapsedTime >= timeLimit && !treasureSpawned) {
        state = GameState::GameOver;
        statusText.setString("Temps ecoule !");
        statusText.setFillColor(sf::Color::Red);
        sf::FloatRect bounds = statusText.getLocalBounds();
        statusText.setOrigin(bounds.left + bounds.width/2.0f, bounds.top + bounds.height/2.0f);
        statusText.setPosition(400.f, 250.f);
        return;
    }

    if (obstacleSpawnTimer > 1.5f) { 
        obstacleSpawnTimer = 0.f;
        spawnEntities();
    }

    player.update(deltaTime, platforms);

    healthBarFill.setSize(sf::Vector2f(100.f * player.getLives(), 20.f)); // Update health width

    for (auto it = platforms.begin(); it != platforms.end(); ) {
        (*it)->update(deltaTime);
        if ((*it)->isOffScreen()) {
            delete *it;
            it = platforms.erase(it);
        } else ++it;
    }

    for (auto it = collectibles.begin(); it != collectibles.end(); ) {
        (*it)->update(deltaTime);
        if (player.getBounds().intersects((*it)->getBounds()) && !(*it)->isCollected()) {
            (*it)->collect();
            bananaScore += 10;
        }
        if ((*it)->isOffScreen() || (*it)->isCollected()) {
            delete *it;
            it = collectibles.erase(it);
        } else ++it;
    }

    for (auto it = obstacles.begin(); it != obstacles.end(); ) {
        (*it)->update(deltaTime); 
        
        if (player.getBounds().intersects((*it)->getBounds())) {
            if (dynamic_cast<Treasure*>(*it)) {
                state = GameState::Win;
                statusText.setString("Victoire ! Tresor trouve !");
                statusText.setFillColor(sf::Color::Green);
                sf::FloatRect bounds = statusText.getLocalBounds();
                statusText.setOrigin(bounds.left + bounds.width/2.0f, bounds.top + bounds.height/2.0f);
                statusText.setPosition(400.f, 250.f);
            } else if (dynamic_cast<Enemy*>(*it) || dynamic_cast<Mine*>(*it) || dynamic_cast<Drone*>(*it) || dynamic_cast<Bird*>(*it)) {
                if (player.takeDamage()) {
                    state = GameState::GameOver;
                    statusText.setString("Game Over ! Mort !");
                    statusText.setFillColor(sf::Color::Red);
                    sf::FloatRect bounds = statusText.getLocalBounds();
                    statusText.setOrigin(bounds.left + bounds.width/2.0f, bounds.top + bounds.height/2.0f);
                    statusText.setPosition(400.f, 250.f);
                }
            }
        }

        if ((*it)->isOffScreen()) {
            delete *it; 
            it = obstacles.erase(it);
        } else ++it;
    }
}

void Game::render() {
    window.clear(sf::Color(20, 20, 30));

    if (hasBgTexture) {
        window.draw(bgSprite1);
        window.draw(bgSprite2);
    }

    if (state == GameState::Menu) {
        window.draw(titleText);
        window.draw(instructionText);
    } else if (state == GameState::About) {
        sf::Text aboutText;
        if (fontLoaded) aboutText.setFont(font);
        aboutText.setCharacterSize(20);
        aboutText.setFillColor(sf::Color::White);
        aboutText.setString("REGLES DU JEU :\n\n- Vous devez atteindre l'abri vivant.\n- Relance via R ou Entree depuis ecran de fin.\n\n[Entree] Retour Menu");
        aboutText.setPosition(50.f, 200.f);
        window.draw(titleText);
        window.draw(aboutText);
    } else if (state == GameState::Playing) {
        window.draw(ground);
        
        for (size_t i = 0; i < platforms.size(); ++i) platforms[i]->draw(window);
        for (size_t i = 0; i < collectibles.size(); ++i) collectibles[i]->draw(window);
        
        player.draw(window);
        
        for (size_t i = 0; i < obstacles.size(); ++i) obstacles[i]->draw(window); 
        
        window.draw(progressBarBackground);
        window.draw(progressBarFill);
        window.draw(scoreText);
        window.draw(healthBarFill);
        window.draw(healthText);
    } else if (state == GameState::GameOver || state == GameState::Win) {
        sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        window.draw(statusText);
        
        sf::Text returnText;
        if (fontLoaded) returnText.setFont(font);
        returnText.setCharacterSize(20);
        returnText.setFillColor(sf::Color::Yellow);
        returnText.setString("[R] pour Rejouer  -  [Entree] pour Menu");
        sf::FloatRect bounds = returnText.getLocalBounds();
        returnText.setOrigin(bounds.left + bounds.width/2.0f, bounds.top + bounds.height/2.0f);
        returnText.setPosition(400.f, 350.f);
        window.draw(returnText);
    }

    window.display();
}
