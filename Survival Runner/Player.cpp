#include <cstdlib>
#include <ctime>
#include <iostream>

#include "Player.hpp"
// ==========================================
// IMPLEMENTATION OBSTACLE
// ==========================================
Obstacle::Obstacle(ObstacleType t, float startX, float s, sf::Texture* texture)
    : type(t), speed(s), active(true) {
  if (type == ObstacleType::Fire) {
    shape.setSize(sf::Vector2f(40.0f, 40.0f));
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(startX, 460.0f);
  } else if (type == ObstacleType::Bird) {
    shape.setSize(sf::Vector2f(50.0f, 30.0f));
    shape.setFillColor(sf::Color::Yellow);
    shape.setPosition(startX, 330.0f);
  } else if (type == ObstacleType::Hole) {
    shape.setSize(sf::Vector2f(60.0f, 20.0f));
    shape.setFillColor(sf::Color::Black);
    shape.setPosition(startX, 490.0f);
  }

  if (texture) {
    shape.setTexture(texture);
    shape.setFillColor(sf::Color::White);
  }
}

void Obstacle::update(float deltaTime) {
  shape.move(-speed * deltaTime, 0.0f);
  if (shape.getPosition().x < -100.0f) {
    active = false;
  }
}

// ==========================================
// IMPLEMENTATION PLAYER
// ==========================================
Player::Player() {
  shape.setSize(sf::Vector2f(50.0f, 100.0f));
  shape.setFillColor(sf::Color::Blue);
  shape.setPosition(100.0f, floorY - shape.getSize().y + 50.0f);
  state = PlayerState::Running;
  health = 2;
  yVelocity = 0.0f;
  speedMultiplier = 1.0f;
}

void Player::setTexture(sf::Texture* texture) {
  if (texture) {
    shape.setTexture(texture);
    shape.setFillColor(sf::Color::White);
  }
}

void Player::handleInput() {
  if (state != PlayerState::Jumping) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
      jump();
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      duck();
    } else {
      stand();
    }
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    speedMultiplier = 1.5f;
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
    speedMultiplier = 0.5f;
  } else {
    speedMultiplier = 1.0f;
  }
}

void Player::jump() {
  state = PlayerState::Jumping;
  yVelocity = jumpVelocity;
  stand();
}

void Player::duck() {
  if (state != PlayerState::Ducking) {
    state = PlayerState::Ducking;
    shape.setSize(sf::Vector2f(50.0f, 50.0f));
    shape.setPosition(100.0f, floorY);
  }
}

void Player::stand() {
  if (state == PlayerState::Ducking) {
    state = PlayerState::Running;
    shape.setSize(sf::Vector2f(50.0f, 100.0f));
    shape.setPosition(100.0f, floorY - 50.0f);
  }
}

void Player::update(float deltaTime) {
  if (state == PlayerState::Jumping) {
    yVelocity += gravity * deltaTime;
    shape.move(0, yVelocity * deltaTime);

    if (shape.getPosition().y + shape.getSize().y >= floorY + 50.0f) {
      shape.setPosition(100.0f, floorY - shape.getSize().y + 50.0f);
      yVelocity = 0.0f;
      state = PlayerState::Running;
    }
  }

  if (health == 2)
    shape.setFillColor(sf::Color::White);
  else if (health == 1)
    shape.setFillColor(sf::Color(255, 100, 100));
}

// ==========================================
// IMPLEMENTATION GAME
// ==========================================
Game::Game()
    : window(sf::VideoMode(800, 600), "Spiderman Rush"),
      currentState(GameState::Menu),
      selectedMenuIndex(0) {
  std::srand(static_cast<unsigned>(std::time(nullptr)));
  window.setFramerateLimit(60);

  if (!font.loadFromFile("arial.ttf")) {
    throw std::runtime_error("Erreur: Impossible de charger arial.ttf.");
  }

  initAssets();
  initUI();
  resetGame();
}

void Game::initAssets() {
  texSpiderman.loadFromFile("spiderman.png");
  texBird.loadFromFile("bird.png");
  texFire.loadFromFile("fire.png");
  texHole.loadFromFile("hole.png");
  texGround.loadFromFile("terre.png");

  if (bgMusic.openFromFile("music.ogg")) {
    bgMusic.setLoop(true);
    bgMusic.setVolume(50.f);
    bgMusic.play();
  } else {
    std::cerr << "Attention: Fichier 'music.ogg' introuvable." << std::endl;
  }
}

void Game::initUI() {
  titleText.setFont(font);
  titleText.setString("SPIDERMAN RUSH");
  titleText.setCharacterSize(50);
  titleText.setPosition(200.0f, 100.0f);

  menuPlayText.setFont(font);
  menuPlayText.setString("1. Jouer");
  menuPlayText.setPosition(350.0f, 250.0f);

  menuAboutText.setFont(font);
  menuAboutText.setString("2. A propos");
  menuAboutText.setPosition(350.0f, 300.0f);

  menuQuitText.setFont(font);
  menuQuitText.setString("3. Quitter");
  menuQuitText.setPosition(350.0f, 350.0f);

  infoText.setFont(font);
  infoText.setCharacterSize(24);
  infoText.setPosition(50.0f, 200.0f);

  hudTimeText.setFont(font);
  hudTimeText.setCharacterSize(20);
  hudTimeText.setPosition(20.0f, 20.0f);

  progressBarBackground.setSize(sf::Vector2f(400.0f, 20.0f));
  progressBarBackground.setFillColor(sf::Color::White);
  progressBarBackground.setPosition(200.0f, 20.0f);

  progressBarFill.setSize(sf::Vector2f(0.0f, 20.0f));
  progressBarFill.setFillColor(sf::Color::Blue);
  progressBarFill.setPosition(200.0f, 20.0f);
}

void Game::resetGame() {
  player = Player();
  if (texSpiderman.getSize().x > 0) {
    player.setTexture(&texSpiderman);
  }

  obstacles.clear();
  timeRemaining = maxTime;
  distanceCovered = 0.0f;
  obstacleSpawnTimer = 0.0f;
}

void Game::spawnObstacle() {
  int typeRand = std::rand() % 3;
  ObstacleType type;
  sf::Texture* obsTexture = nullptr;

  if (typeRand == 0) {
    type = ObstacleType::Fire;
    if (texFire.getSize().x > 0) obsTexture = &texFire;
  } else if (typeRand == 1) {
    type = ObstacleType::Bird;
    if (texBird.getSize().x > 0) obsTexture = &texBird;
  } else {
    type = ObstacleType::Hole;
    if (texHole.getSize().x > 0) obsTexture = &texHole;
  }

  float baseSpeed = 300.0f + (std::rand() % 200);
  obstacles.emplace_back(type, 850.0f, baseSpeed, obsTexture);
}

void Game::processEvents() {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) window.close();

    if (event.type == sf::Event::KeyPressed) {
      if (currentState == GameState::Menu) {
        if (event.key.code == sf::Keyboard::Num1 ||
            event.key.code == sf::Keyboard::Numpad1) {
          resetGame();
          currentState = GameState::Playing;
        } else if (event.key.code == sf::Keyboard::Num2 ||
                   event.key.code == sf::Keyboard::Numpad2) {
          currentState = GameState::About;
        } else if (event.key.code == sf::Keyboard::Num3 ||
                   event.key.code == sf::Keyboard::Numpad3) {
          window.close();
        }
      } else if (currentState == GameState::About) {
        if (event.key.code == sf::Keyboard::Escape) {
          currentState = GameState::Menu;
        }
      } else if (currentState == GameState::GameOver ||
                 currentState == GameState::Win) {
        if (event.key.code == sf::Keyboard::Enter) {
          currentState = GameState::Menu;
        }
      }
    }
  }
}

void Game::update(float deltaTime) {
  if (currentState == GameState::Playing) {
    player.handleInput();
    player.update(deltaTime);

    timeRemaining -= deltaTime;
    distanceCovered += (200.0f * player.speedMultiplier) * deltaTime;

    float progressPct = distanceCovered / maxDistance;
    progressBarFill.setSize(sf::Vector2f(400.0f * progressPct, 20.0f));

    if (timeRemaining <= 0.0f) {
      currentState = GameState::GameOver;
    } else if (distanceCovered >= maxDistance) {
      currentState = GameState::Win;
    }

    obstacleSpawnTimer -= deltaTime;
    if (obstacleSpawnTimer <= 0.0f) {
      spawnObstacle();
      obstacleSpawnTimer =
          1.0f + static_cast<float>(std::rand() % 200) / 100.0f;
    }

    for (auto& obs : obstacles) {
      if (!obs.active) continue;
      obs.update(deltaTime * player.speedMultiplier);

      if (player.shape.getGlobalBounds().intersects(
              obs.shape.getGlobalBounds())) {
        obs.active = false;

        if (obs.type == ObstacleType::Hole) {
          player.health = 0;
        } else {
          player.health--;
        }

        if (player.health <= 0) {
          currentState = GameState::GameOver;
        }
      }
    }
  }
}

void Game::render() {
  window.clear(sf::Color(135, 206, 235));

  if (currentState == GameState::Menu) {
    window.draw(titleText);
    window.draw(menuPlayText);
    window.draw(menuAboutText);
    window.draw(menuQuitText);
  } else if (currentState == GameState::About) {
    infoText.setString(
        "A PROPOS DU JEU:\n\nCourez vers l'abri avant la fin du temps!\nFleche "
        "HAUT: Sauter\nFleche BAS: Se baisser\nFleches GAUCHE/DROITE: Ajuster "
        "la vitesse\n\nAppuyez sur ECHAP pour retourner.");
    window.draw(infoText);
  } else if (currentState == GameState::Playing) {
    sf::RectangleShape floorShape(sf::Vector2f(800.0f, 100.0f));
    floorShape.setPosition(0.0f, 500.0f);
    if (texGround.getSize().x > 0) {
      floorShape.setTexture(&texGround);
    } else {
      floorShape.setFillColor(sf::Color(139, 69, 19));
    }
    window.draw(floorShape);

    window.draw(player.shape);
    for (const auto& obs : obstacles) {
      if (obs.active) window.draw(obs.shape);
    }

    hudTimeText.setString("Temps: " + std::to_string((int)timeRemaining) + "s");
    window.draw(hudTimeText);
    window.draw(progressBarBackground);
    window.draw(progressBarFill);
  } else if (currentState == GameState::GameOver) {
    infoText.setString(
        "GAME OVER\nVous avez heurte trop d'obstacles ou le temps est "
        "ecoule!\nAppuyez sur ENTREE pour retourner au menu.");
    window.draw(infoText);
  } else if (currentState == GameState::Win) {
    infoText.setString(
        "VICTOIRE !\nVous avez atteint le bunker sain et sauf.\nAppuyez sur "
        "ENTREE pour retourner au menu.");
    window.draw(infoText);
  }

  window.display();
}

void Game::run() {
  sf::Clock clock;
  while (window.isOpen()) {
    float deltaTime = clock.restart().asSeconds();
    processEvents();
    update(deltaTime);
    render();
  }
}