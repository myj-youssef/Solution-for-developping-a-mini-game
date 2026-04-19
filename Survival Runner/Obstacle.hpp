#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include <SFML/Graphics.hpp>
#include <iostream>

/**
 * @brief Classe de base Obstacle
 * Concrétise le SRP et l'Abstraction avec une gestion mémoire par pointage brut.
 */
class Obstacle {
protected:
    float speed;
    sf::Sprite sprite;
    bool hasTexture;
    sf::Shape* fallbackShape; // Allocation dynamique requise

public:
    Obstacle();
    virtual ~Obstacle(); // Nettoie le fallbackShape polymorphiquement

    virtual void update(float deltaTime);
    virtual void draw(sf::RenderWindow& window);
    virtual sf::FloatRect getBounds() const;
    virtual bool isOffScreen() const;
};

class Enemy : public Obstacle {
public:
    Enemy(float groundHeight, float startX, const sf::Texture& tex, bool isValidTex);
    ~Enemy() override;
    sf::FloatRect getBounds() const override; // Hitbox rétrécie
};

class Treasure : public Obstacle {
public:
    Treasure(float airHeight, float startX, const sf::Texture& tex, bool isValidTex);
    ~Treasure() override;
    sf::FloatRect getBounds() const override; // Hitbox rétrécie
};

class Mine : public Obstacle {
public:
    Mine(float groundHeight, float startX, const sf::Texture& tex, bool isValidTex);
    ~Mine() override;
};

class Drone : public Obstacle {
public:
    Drone(float airHeight, float startX, const sf::Texture& tex, bool isValidTex);
    ~Drone() override;
};

class Bird : public Obstacle {
private:
    float startY;
    float timeAlive;
public:
    Bird(float airHeight, float startX, const sf::Texture& tex, bool isValidTex);
    ~Bird() override;
    void update(float deltaTime) override; // Comportement Kamikaze unique
};

#endif // OBSTACLE_HPP
