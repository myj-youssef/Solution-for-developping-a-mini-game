#pragma once

#include <SFML/Graphics.hpp>

class Player {
private:
    sf::RectangleShape fallbackShape;
    sf::Sprite sprite;
    
    bool hasTextures;
    const sf::Texture* texRun1; 
    const sf::Texture* texRun2;
    
    float animTimer; 
    sf::Vector2f velocity;
    
    bool isJumping;
    bool isDucking;
    
    const float groundLevel;
    const float gravity;
    const float jumpForce;
    const float moveSpeed;
    const sf::Vector2f normalSize;
    const sf::Vector2f duckSize;

public:
    Player(float startX, float groundY, const sf::Texture& t1, const sf::Texture& t2, bool tLoaded);
    
    void update(float deltaTime, bool isRunning);
    void draw(sf::RenderWindow& window);
    
    void run(float directionMultiplier); 
    void jump();
    void duck();
    void standUp();
    void reset();
    
    // Déduction des hitbox précises
    sf::FloatRect getHitbox() const;
};
