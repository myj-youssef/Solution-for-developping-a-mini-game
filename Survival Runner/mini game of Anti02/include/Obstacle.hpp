#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include <SFML/Graphics.hpp>
#include <iostream>

/**
 * @brief Classe de base Obstacle (Abstraite).
 * Exige l'implémentation de draw() et update() pour le Polymorphisme dynamique.
 */
class Obstacle {
protected:
    float speed;
    sf::Sprite sprite;
    bool hasTexture; // On ne stocke plus sf::Texture ici pour préserver la RAM

public:
    Obstacle();
    virtual ~Obstacle(); // Destructeur Virtuel requis pour l'implémentation de Game::~Game()

    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual bool isOffScreen() const = 0;
};

/**
 * @brief Classe Enemy (ex Mine), dérive publiquement de Obstacle.
 */
class Enemy : public Obstacle {
private:
    sf::RectangleShape fallbackShape;
public:
    // Passage par référence constante pour la texture !
    Enemy(float groundHeight, float startX, const sf::Texture& tex, bool isValidTex);
    ~Enemy() override;
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool isOffScreen() const override;
};

/**
 * @brief Classe Treasure (ex Drone), dérive publiquement de Obstacle.
 */
class Treasure : public Obstacle {
private:
    sf::CircleShape fallbackShape;
public:
    // Passage par référence constante pour la texture !
    Treasure(float airHeight, float startX, const sf::Texture& tex, bool isValidTex);
    ~Treasure() override;
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool isOffScreen() const override;
};

class Mine : public Obstacle {
private:
    sf::RectangleShape fallbackShape;
public:
    Mine(float groundHeight, float startX, const sf::Texture& tex, bool isValidTex);
    ~Mine() override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool isOffScreen() const override;
};

class Drone : public Obstacle {
private:
    sf::RectangleShape fallbackShape;
public:
    Drone(float airHeight, float startX, const sf::Texture& tex, bool isValidTex);
    ~Drone() override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool isOffScreen() const override;
};

class Bird : public Obstacle {
private:
    sf::RectangleShape fallbackShape;
    float startY;
    float timeAlive;
public:
    Bird(float airHeight, float startX, const sf::Texture& tex, bool isValidTex);
    ~Bird() override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool isOffScreen() const override;
};

#endif // OBSTACLE_HPP
