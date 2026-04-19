#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "Player.hpp"
#include "Obstacle.hpp"
#include "Collectible.hpp"
#include "Platform.hpp"

enum class GameState { Menu, About, Playing, GameOver, Win };

/**
 * @brief Gestionnaire principal du jeu (Moteur).
 * Encapsule la fenêtre, les entités, l'UI, et la Game Loop.
 * Gère le chargement centralisé des textures (Chapitre 07 - Optimisation) pour
 * les passer par référence constante aux objets instanciés et éviter les copies inutiles.
 */
class Game {
private:
    sf::RenderWindow window;
    GameState state;
    
    // Ressources partagées
    sf::Font font;
    bool fontLoaded;
    sf::Texture bgTexture;
    sf::Sprite bgSprite1;
    sf::Sprite bgSprite2;
    bool hasBgTexture;
    
    // --- TEXTURES CENTRALISÉES ---
    sf::Texture playerTexRun1;
    sf::Texture playerTexRun2;
    sf::Texture playerTexDuck;
    bool hasPRun1, hasPRun2, hasPDuck;

    sf::Texture enemyTex;
    bool hasEnemyTex;

    sf::Texture mineTex;
    bool hasMineTex;
    
    sf::Texture droneTex;
    bool hasDroneTex;

    sf::Texture birdTex;
    bool hasBirdTex;

    
    sf::Texture platformTex;
    bool hasPlatformTex;

    sf::Texture collectibleTex;
    bool hasCollectibleTex;

    sf::Texture treasureTex;
    bool hasTreasureTex;

    // --- AUDIO ---
    sf::Music bgMusic;
    
    // Entités
    Player player;
    std::vector<Obstacle*> obstacles; 
    std::vector<Platform*> platforms;
    std::vector<Collectible*> collectibles;
    
    // Temps et progression
    sf::Clock clock;
    float obstacleSpawnTimer;
    float timeLimit;
    float elapsedTime;
    int bananaScore;
    bool treasureSpawned;
    
    // UI Elements
    sf::Text titleText;
    sf::Text instructionText;
    sf::Text statusText;
    sf::Text scoreText;
    sf::RectangleShape progressBarBackground;
    sf::RectangleShape progressBarFill;
    sf::RectangleShape healthBarFill;
    sf::Text healthText;
    sf::Text timerText;


    void loadAssets();
    void initUI();
    void resetGame();
    void spawnEntities();

public:
    Game();
    ~Game(); // Nettoyage de la mémoire

    void run();
    void processEvents();
    void update(float deltaTime);
    void render();
};

#endif // GAME_HPP
