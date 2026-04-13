#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <stdexcept>
#include <string>
#include <vector>

// --- Énumérations ---
enum class GameState { Menu, About, Playing, GameOver, Win };
enum class PlayerState { Running, Jumping, Ducking };
enum class ObstacleType { Fire, Bird, Hole };

// --- Structure Obstacle ---
struct Obstacle {
  sf::RectangleShape shape;
  ObstacleType type;
  float speed;
  bool active;

  Obstacle(ObstacleType t, float startX, float speed, sf::Texture* texture);
  void update(float deltaTime);
};

// --- Classe Player ---
class Player {
 public:
  sf::RectangleShape shape;
  PlayerState state;
  int health;  // 2 = en pleine forme, 1 = blessé, 0 = mort
  float yVelocity;
  float speedMultiplier;

  static constexpr float floorY = 450.0f;
  static constexpr float gravity = 980.0f;
  static constexpr float jumpVelocity = -600.0f;

  Player();
  void setTexture(sf::Texture* texture);
  void update(float deltaTime);
  void jump();
  void duck();
  void stand();
  void handleInput();
};

// --- Classe principale Game ---
class Game {
 private:
  sf::RenderWindow window;
  GameState currentState;
  sf::Font font;

  Player player;
  std::vector<Obstacle> obstacles;

  float obstacleSpawnTimer;
  float timeRemaining;
  float distanceCovered;
  const float maxDistance = 5000.0f;
  const float maxTime = 60.0f;

  // Textures et Musique
  sf::Texture texSpiderman;
  sf::Texture texBird;
  sf::Texture texFire;
  sf::Texture texHole;
  sf::Texture texGround;
  sf::Music bgMusic;

  // Éléments de l'interface (UI)
  sf::Text titleText;
  sf::Text menuPlayText;
  sf::Text menuAboutText;
  sf::Text menuQuitText;
  sf::Text infoText;
  sf::Text hudTimeText;
  sf::RectangleShape progressBarBackground;
  sf::RectangleShape progressBarFill;
  int selectedMenuIndex;

  void initAssets();
  void initUI();
  void resetGame();
  void spawnObstacle();

  void processEvents();
  void update(float deltaTime);
  void render();

 public:
  Game();
  void run();
};