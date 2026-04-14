#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Player.hpp"
#include "Obstacle.hpp"

enum class GameState {
    Menu,
    About,
    Playing,
    GameOver,
    Victory
};

class Game {
private:
    sf::RenderWindow window;
    GameState state;
    
    sf::Font font;
    bool hasFont;
    
    // Debug 
    bool debugMode;

    Player* player; 
    std::vector<Obstacle*> obstacles;
    
    sf::Texture playerRun1Tex;
    sf::Texture playerRun2Tex;
    bool hasPlayerTex;
    
    sf::Texture mineTex;
    bool hasMineTex;
    
    sf::Texture droneTex;
    bool hasDroneTex;
    
    sf::Texture bgTex;
    bool hasBgTex;
    sf::Sprite bgSprite1;
    sf::Sprite bgSprite2;
    float bgScrollSpeed;
    
    float remainingTime;         
    const float totalTimeLimit;  
    float distanceCovered;       
    const float distanceToBunker;
    float score;
    float timeSinceLastObstacle;
    int selectedMenuOption; 

    void processEvents();
    void update(float deltaTime);
    void render();
    void drawUI();
    void drawMenu();
    void drawAbout();
    void drawEndScreen(const std::string& title, const sf::Color& color);
    
    void spawnObstacle();
    void clearObstacles();
    void resetGame();

public:
    Game();
    ~Game(); 
    void run();
};
