#pragma once

#include <SFML/Graphics.hpp>

/**
 * @class Obstacle
 * @brief Classe de base abstraite représentant un obstacle en jeu.
 */
class Obstacle {
public:
    virtual ~Obstacle() {} 
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    // Remplacement de getBounds par getHitbox pour une collision précise (Padding appliqué)
    virtual sf::FloatRect getHitbox() const = 0;
    virtual bool isOffScreen() const = 0;
};

class Mine : public Obstacle {
private:
    float speed;
    sf::Sprite sprite;
    sf::RectangleShape fallbackShape;
    bool hasTexture;

public:
    Mine(float startX, float startY, const sf::Texture& texture, bool textureLoaded);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getHitbox() const override;
    bool isOffScreen() const override;
};

class Drone : public Obstacle {
private:
    float speed;
    sf::Sprite sprite;
    sf::CircleShape fallbackShape;
    bool hasTexture;

public:
    Drone(float startX, float startY, const sf::Texture& texture, bool textureLoaded);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getHitbox() const override;
    bool isOffScreen() const override;
};
