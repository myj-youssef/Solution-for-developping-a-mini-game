#include "Game.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

/**
 * @brief Constructeur Principal du Jeu.
 * Responsabilités L2 : Initialiser la fenêtre SFML, les compteurs de boucle, 
 * et appliquer le pattern Flyweight : on charge les textures une et une seule fois
 * en mémoire (VRAM), et on passe leurs adresses aux entités pour éviter les copies colossales.
 */
Game::Game() 
    : window(sf::VideoMode(800, 600), "Application de Jeu de Cachette - Rendu Final L2"),
      state(GameState::Menu),
      debugMode(false),
      totalTimeLimit(30.0f),
      remainingTime(30.0f),
      distanceCovered(0.0f),
      distanceToBunker(1000.0f),
      score(0.0f),
      timeSinceLastObstacle(0.0f),
      selectedMenuOption(0),
      bgScrollSpeed(100.0f) {
          
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Chargement Font (Sécurisé)
    if (!font.loadFromFile("arial.ttf")) {
        hasFont = false;
        std::cerr << "Erreur: 'arial.ttf' manquant. Les textes critiques ne s'afficheront pas.\n";
    } else {
        hasFont = true;
    }

    // Gestion centralisée des Textures (Flyweight Pattern)
    hasPlayerTex = playerRun1Tex.loadFromFile("assets/player_run1.png") && 
                   playerRun2Tex.loadFromFile("assets/player_run2.png");
    if (!hasPlayerTex) std::cerr << "Erreur: Textures Player manquantes (assets/player_run[1/2].png). Activation fallback block geometrique.\n";

    hasMineTex = mineTex.loadFromFile("assets/mine.png");
    if (!hasMineTex) std::cerr << "Erreur: Texture Mine (assets/mine.png) manquante. Activation fallback couleur.\n";

    hasDroneTex = droneTex.loadFromFile("assets/drone.png");
    if (!hasDroneTex) std::cerr << "Erreur: Texture Drone (assets/drone.png) manquante. Activation fallback couleur.\n";

    hasBgTex = bgTex.loadFromFile("assets/background.png");
    if (!hasBgTex) {
        std::cerr << "Erreur: Texture de fond d'ecran (assets/background.png) manquante.\n";
    } else {
        // Paramétrage du Parallax statique (2 sprites mis côte-à-côte pour créer un mouvement infini)
        bgSprite1.setTexture(bgTex);
        bgSprite2.setTexture(bgTex);
        bgSprite2.setPosition(static_cast<float>(bgTex.getSize().x), 0.0f);
    }
    
    // Instanciation Manuelle du Joueur : On transmet de façon très optimisée les textures constantes.
    player = new Player(100.0f, 500.0f, playerRun1Tex, playerRun2Tex, hasPlayerTex);
}

/**
 * @brief Destructeur de Game.
 * Fondamental en C++ : Empêche toute fuite de mémoire (Memory Leak) 
 * en utilisant 'delete' sur tous les objets instanciés via 'new'.
 */
Game::~Game() {
    delete player;
    clearObstacles(); 
}

/**
 * @brief Vide la mémoire des pointeurs bruts d'obstacles restants.
 */
void Game::clearObstacles() {
    for (Obstacle* obs : obstacles) {
        delete obs;
    }
    obstacles.clear();
}

/**
 * @brief Réinitialise l'état du jeu pour une nouvelle partie.
 */
void Game::resetGame() {
    player->reset();
    clearObstacles();
    remainingTime = totalTimeLimit;
    distanceCovered = 0.0f;
    timeSinceLastObstacle = 0.0f;
    score = 0.0f;
}

/**
 * @brief Game Loop Standard L2 (Event -> Update -> Render)
 */
void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

/**
 * @brief Moteur Evénementiel.
 * Gère tout ce qui est pression instantanée (menus, sauts, etc...).
 */
void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (state == GameState::Menu) {
                if (event.key.code == sf::Keyboard::Up) {
                    selectedMenuOption = (selectedMenuOption > 0) ? selectedMenuOption - 1 : 2;
                } else if (event.key.code == sf::Keyboard::Down) {
                    selectedMenuOption = (selectedMenuOption < 2) ? selectedMenuOption + 1 : 0;
                } else if (event.key.code == sf::Keyboard::Enter) {
                    if (selectedMenuOption == 0) {
                        resetGame();
                        state = GameState::Playing;
                    } else if (selectedMenuOption == 1) {
                        state = GameState::About;
                    } else if (selectedMenuOption == 2) {
                        window.close();
                    }
                }
            } else if (state == GameState::About) {
                if (event.key.code == sf::Keyboard::Escape) {
                    state = GameState::Menu;
                }
            } else if (state == GameState::Playing) {
                if (event.key.code == sf::Keyboard::D) {
                    debugMode = !debugMode;
                }
                
                // Actions ponctuelles
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Space) {
                    player->jump();
                } else if (event.key.code == sf::Keyboard::Down) {
                    player->duck();
                }
            } else if (state == GameState::GameOver || state == GameState::Victory) {
                // Possibilité de relancer en appuyant sur Entrée ou R comme demandé.
                if (event.key.code == sf::Keyboard::Escape) {
                    state = GameState::Menu;
                } else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::R) {
                    resetGame();
                    state = GameState::Playing;
                }
            }
        }
        
        // Libération de la position baissée
        if (event.type == sf::Event::KeyReleased && state == GameState::Playing) {
            if (event.key.code == sf::Keyboard::Down) {
                player->standUp();
            }
        }
    }
}

/**
 * @brief Logique mathématique de mise à jour.
 * Avance le temps, scrute les inputs continus (Courir), active le polymorphisme, 
 * gère la mécanique de Scrolling du fond et le système de score.
 */
void Game::update(float deltaTime) {
    if (state != GameState::Playing) return;

    // Défaite si temps expiré
    remainingTime -= deltaTime;
    score += deltaTime * 100.0f; 

    if (remainingTime <= 0.0f) {
        state = GameState::GameOver;
        return;
    }

    // Gestion du déplacement via clavier continu
    bool isRunning = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        player->run(1.0f); 
        distanceCovered += 100.0f * deltaTime; 
        isRunning = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        player->run(-1.0f);
        isRunning = true; // On joue l'animation même en reculant
    } else {
        player->run(0.0f);
    }

    if (distanceCovered >= distanceToBunker) {
        state = GameState::Victory;
        return;
    }

    // Update logique du joueur (et son animation)
    player->update(deltaTime, isRunning);

    // Défilement du Fond (Scrolling) si présence image
    if (hasBgTex) {
        // Le fond bouge légèrement plus vite si le joueur court vers la droite (effet supplémentaire)
        float currentScrollSpeed = isRunning ? bgScrollSpeed * 1.5f : bgScrollSpeed;
        
        bgSprite1.move(-currentScrollSpeed * deltaTime, 0.0f);
        bgSprite2.move(-currentScrollSpeed * deltaTime, 0.0f);
        
        // Wrap-around : si un sprite sort totalement par la gauche, on le replace à l'arrière du deuxième
        float textureWidth = static_cast<float>(bgTex.getSize().x);
        if (bgSprite1.getPosition().x + textureWidth < 0.0f) {
            bgSprite1.setPosition(bgSprite2.getPosition().x + textureWidth, 0.0f);
        }
        if (bgSprite2.getPosition().x + textureWidth < 0.0f) {
            bgSprite2.setPosition(bgSprite1.getPosition().x + textureWidth, 0.0f);
        }
    }

    // Apparition des Obstacles
    timeSinceLastObstacle += deltaTime;
    if (timeSinceLastObstacle >= 1.2f) {
        spawnObstacle();
        timeSinceLastObstacle = 0.0f;
    }

    // Exploitation Polymorphique de chaque Obstacle
    for (auto it = obstacles.begin(); it != obstacles.end();) {
        Obstacle* obs = *it;
        obs->update(deltaTime); 
        
        // Nouvelle Hitbox réduite de 10% pour pardonner aux bords
        if (obs->getHitbox().intersects(player->getHitbox())) {
            state = GameState::GameOver;
            return;
        }

        if (obs->isOffScreen()) {
            delete obs; // Nettoyage impératif C++ niveau S4
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }
}

/**
 * @brief Usine de création (Pattern Factory simple).
 * Distribution des références de Textures aux entités instanciées.
 */
void Game::spawnObstacle() {
    // Calcul du Safe Spawn (minimum 300px du joueur)
    float playerRightEdge = player->getHitbox().left + player->getHitbox().width;
    float safeSpawnX = std::max(800.0f, playerRightEdge + 300.0f);

    int r = std::rand() % 2;
    if (r == 0) {
        obstacles.push_back(new Mine(safeSpawnX, 500.0f, mineTex, hasMineTex));
    } else {
        obstacles.push_back(new Drone(safeSpawnX, 430.0f, droneTex, hasDroneTex));
    }
}

/**
 * @brief Moteur de Rendu final de l'image.
 * L'ordre Z-Index (Z-buffer) est respecté de haut en bas: Ciel -> Sol -> Entités -> UI.
 */
void Game::render() {
    // Couleur par défaut si aucun Sprite n'est rendu
    window.clear(sf::Color(135, 206, 235)); 

    if (state == GameState::Menu) {
        drawMenu();
    } else if (state == GameState::About) {
        drawAbout();
    } else if (state == GameState::Playing) {
        
        // 1. Fond (Ciel / Décor)
        if (hasBgTex) {
            window.draw(bgSprite1);
            window.draw(bgSprite2);
        }

        // 2. Sol Fixe
        sf::RectangleShape ground(sf::Vector2f(800.0f, 100.0f));
        ground.setPosition(0.0f, 500.0f);
        ground.setFillColor(sf::Color(34, 139, 34)); // Type herbe
        window.draw(ground);

        // 3. Entités
        player->draw(window);
        // Le polymorphisme est effectif ici (draw est purement virtuel et résolu à l'execution)
        for (Obstacle* obs : obstacles) {
            obs->draw(window);
        }
        
        // 4. Debug Mode Optionnel (Hitboxes visuelles)
        if (debugMode) {
            auto drawRect = [&](const sf::FloatRect& r) {
                sf::RectangleShape debugBox(sf::Vector2f(r.width, r.height));
                debugBox.setPosition(r.left, r.top);
                debugBox.setFillColor(sf::Color::Transparent);
                debugBox.setOutlineColor(sf::Color::Red);
                debugBox.setOutlineThickness(2.0f);
                window.draw(debugBox);
            };
            drawRect(player->getHitbox());
            for (Obstacle* obs : obstacles) {
                drawRect(obs->getHitbox());
            }
        }

        // 5. Interface (HUD)
        drawUI();
        
    } else if (state == GameState::Victory) {
        drawEndScreen("Victoire ! Vous etes a l'abri (Bunker) !", sf::Color::Green);
    } else if (state == GameState::GameOver) {
        if (remainingTime <= 0.0f) {
            drawEndScreen("Defaite ! Temps ecoule.", sf::Color::Red);
        } else {
            drawEndScreen("Defaite ! Touche par un obstacle.", sf::Color::Red);
        }
    }

    window.display();
}

void Game::drawMenu() {
    if (!hasFont) return;
    
    sf::Text title("Jeu de Cachette", font, 50);
    title.setFillColor(sf::Color::Black);
    title.setPosition(200.0f, 100.0f);
    window.draw(title);

    std::string options[3] = {"Jouer", "A Propos", "Quitter"};
    for (int i = 0; i < 3; ++i) {
        sf::Text txt(options[i], font, 30);
        if (i == selectedMenuOption) {
            txt.setFillColor(sf::Color::Red);
            txt.setString("> " + options[i] + " <");
            txt.setPosition(280.0f, 250.0f + i * 50.0f);
        } else {
            txt.setFillColor(sf::Color::Black);
            txt.setPosition(300.0f, 250.0f + i * 50.0f);
        }
        window.draw(txt);
    }
}

void Game::drawAbout() {
    if (!hasFont) return;
    
    sf::Text title("A Propos & Regles", font, 40);
    title.setFillColor(sf::Color::Black);
    title.setPosition(250.0f, 50.0f);
    window.draw(title);
    
    sf::Text desc(
        "But du jeu:\n"
        "Atteindre le bunker en courant vers la droite.\n"
        "Si le chrono atteint 0 avant d'arriver, vous perdez.\n\n"
        "Controles:\n"
        "- Fleche Droite: Courir et progresser vers l'abri\n"
        "- Fleche Haut/Espace: Sauter (Esquiver Mines au sol)\n"
        "- Fleche Bas: Se baisser (Esquiver Drones volants)\n\n"
        "Appuyez sur Echap pour retourner au menu.", font, 20);
    desc.setFillColor(sf::Color::White);
    desc.setOutlineColor(sf::Color::Black);
    desc.setOutlineThickness(1.5f);
    desc.setPosition(80.0f, 150.0f);
    window.draw(desc);
}

void Game::drawUI() {
    if (!hasFont) return;
    
    // Chronomètre
    std::string timeStr = "Temps : " + std::to_string(static_cast<int>(remainingTime)) + "s";
    sf::Text timeText(timeStr, font, 24);
    timeText.setFillColor(sf::Color::Black);
    timeText.setPosition(20.0f, 20.0f);
    window.draw(timeText);

    // Score
    std::string scoreStr = "Score : " + std::to_string(static_cast<int>(score));
    sf::Text scoreText(scoreStr, font, 24);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setPosition(400.0f, 20.0f); // Affiché en haut, centré/droite
    window.draw(scoreText);
    
    // Barre de progression (Fond)
    sf::RectangleShape pBarBackground(sf::Vector2f(300.0f, 20.0f));
    pBarBackground.setFillColor(sf::Color::Black);
    pBarBackground.setPosition(20.0f, 60.0f);
    window.draw(pBarBackground);
    
    // Remplissage dynamique proportionnel
    float progressRatio = distanceCovered / distanceToBunker;
    if (progressRatio > 1.0f) progressRatio = 1.0f;
    
    sf::RectangleShape pBarForeground(sf::Vector2f(300.0f * progressRatio, 20.0f));
    pBarForeground.setFillColor(sf::Color::Yellow);
    pBarForeground.setPosition(20.0f, 60.0f);
    window.draw(pBarForeground);
    
    sf::Text bunkerText("Bunker", font, 14);
    bunkerText.setFillColor(sf::Color::White);
    bunkerText.setPosition(330.0f, 61.0f);
    window.draw(bunkerText);
}

void Game::drawEndScreen(const std::string& titleStr, const sf::Color& color) {
    if (!hasFont) return;
    
    sf::Text title(titleStr, font, 35);
    title.setFillColor(color);
    title.setPosition(100.0f, 250.0f);
    window.draw(title);
    
    sf::Text subtitle("Entree (ou 'R') pour rejouer, Echap pour revenir au menu", font, 24);
    subtitle.setFillColor(sf::Color::Black);
    subtitle.setPosition(90.0f, 320.0f);
    window.draw(subtitle);
}
