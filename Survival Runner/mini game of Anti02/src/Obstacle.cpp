#include "../include/Obstacle.hpp"

// --- OBSTACLE BASE ---
Obstacle::Obstacle() : speed(350.f), hasTexture(false) {}
Obstacle::~Obstacle() {}

// --- MINE ---
/**
 * @brief Constructeur de Enemy utilisant le passage de texture par theGame
 */
Enemy::Enemy(float groundHeight, float startX, const sf::Texture& tex, bool isValidTex) {
    fallbackShape.setSize(sf::Vector2f(40.f, 30.f));
    fallbackShape.setFillColor(sf::Color::Red);
    fallbackShape.setPosition(startX, groundHeight - fallbackShape.getSize().y);
    
    hasTexture = isValidTex;
    if (hasTexture) {
        sprite.setTexture(tex);
        float sX = 100.f / tex.getSize().x;
        float sY = 80.f / tex.getSize().y;
        sprite.setScale(sX, sY);
        sprite.setPosition(startX, groundHeight - sprite.getGlobalBounds().height);
    }
}

Enemy::~Enemy() {}

void Enemy::update(float deltaTime) {
    if (hasTexture) sprite.move(-speed * deltaTime, 0.f);
    fallbackShape.move(-speed * deltaTime, 0.f);
}

void Enemy::draw(sf::RenderWindow& window) {
    if (hasTexture) window.draw(sprite);
    else window.draw(fallbackShape);
}

sf::FloatRect Enemy::getBounds() const {
    if (hasTexture) {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        bounds.left += bounds.width * 0.1f;
        bounds.width *= 0.8f;
        bounds.top += bounds.height * 0.2f;
        bounds.height *= 0.8f;
        return bounds;
    }
    return fallbackShape.getGlobalBounds();
}

bool Enemy::isOffScreen() const {
    float x = hasTexture ? sprite.getPosition().x + sprite.getGlobalBounds().width : fallbackShape.getPosition().x + fallbackShape.getSize().x;
    return x < 0;
}

// --- DRONE ---
/**
 * @brief Constructeur de Treasure utilisant le passage de texture
 */
Treasure::Treasure(float airHeight, float startX, const sf::Texture& tex, bool isValidTex) {
    fallbackShape.setRadius(20.f);
    fallbackShape.setFillColor(sf::Color(255, 165, 0));
    fallbackShape.setPosition(startX, airHeight);
    
    hasTexture = isValidTex;
    if (hasTexture) {
        sprite.setTexture(tex);
        float sX = 60.f / tex.getSize().x;
        float sY = 60.f / tex.getSize().y;
        sprite.setScale(sX, sY);
        sprite.setPosition(startX, airHeight);
    }
}

Treasure::~Treasure() {}

void Treasure::update(float deltaTime) {
    float droneSpeed = speed * 1.3f;
    if (hasTexture) sprite.move(-droneSpeed * deltaTime, 0.f);
    fallbackShape.move(-droneSpeed * deltaTime, 0.f);
}

void Treasure::draw(sf::RenderWindow& window) {
    if (hasTexture) window.draw(sprite);
    else window.draw(fallbackShape);
}

sf::FloatRect Treasure::getBounds() const {
    if (hasTexture) {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        bounds.left += bounds.width * 0.2f;
        bounds.width *= 0.6f;
        bounds.top += bounds.height * 0.2f;
        bounds.height *= 0.6f;
        return bounds;
    }
    return fallbackShape.getGlobalBounds();
}

bool Treasure::isOffScreen() const {
    float x = hasTexture ? sprite.getPosition().x + sprite.getGlobalBounds().width : fallbackShape.getPosition().x + fallbackShape.getRadius() * 2.f;
    return x < 0;
}

// --- MINE ---
Mine::Mine(float groundHeight, float startX, const sf::Texture& tex, bool isValidTex) {
    fallbackShape.setSize(sf::Vector2f(40.f, 40.f));
    fallbackShape.setFillColor(sf::Color(100, 100, 100));
    fallbackShape.setPosition(startX, groundHeight - fallbackShape.getSize().y);
    hasTexture = isValidTex;
    if (hasTexture) {
        sprite.setTexture(tex);
        sprite.setScale(50.f / tex.getSize().x, 50.f / tex.getSize().y);
        sprite.setPosition(startX, groundHeight - sprite.getGlobalBounds().height);
    }
}
Mine::~Mine() {}
void Mine::update(float deltaTime) {
    if (hasTexture) sprite.move(-speed * deltaTime, 0.f);
    fallbackShape.move(-speed * deltaTime, 0.f);
}
void Mine::draw(sf::RenderWindow& window) {
    if (hasTexture) window.draw(sprite); else window.draw(fallbackShape);
}
sf::FloatRect Mine::getBounds() const {
    if (hasTexture) return sprite.getGlobalBounds();
    return fallbackShape.getGlobalBounds();
}
bool Mine::isOffScreen() const {
    return (hasTexture ? sprite.getPosition().x + sprite.getGlobalBounds().width : fallbackShape.getPosition().x + fallbackShape.getSize().x) < 0;
}

// --- DRONE ---
Drone::Drone(float airHeight, float startX, const sf::Texture& tex, bool isValidTex) {
    fallbackShape.setSize(sf::Vector2f(50.f, 30.f));
    fallbackShape.setFillColor(sf::Color::Cyan);
    fallbackShape.setPosition(startX, airHeight);
    hasTexture = isValidTex;
    if (hasTexture) {
        sprite.setTexture(tex);
        sprite.setScale(70.f / tex.getSize().x, 40.f / tex.getSize().y);
        sprite.setPosition(startX, airHeight - sprite.getGlobalBounds().height);
    }
}
Drone::~Drone() {}
void Drone::update(float deltaTime) {
    if (hasTexture) sprite.move(-speed * 1.2f * deltaTime, 0.f);
    fallbackShape.move(-speed * 1.2f * deltaTime, 0.f);
}
void Drone::draw(sf::RenderWindow& window) {
    if (hasTexture) window.draw(sprite); else window.draw(fallbackShape);
}
sf::FloatRect Drone::getBounds() const {
    if (hasTexture) return sprite.getGlobalBounds();
    return fallbackShape.getGlobalBounds();
}
bool Drone::isOffScreen() const {
    return (hasTexture ? sprite.getPosition().x + sprite.getGlobalBounds().width : fallbackShape.getPosition().x + fallbackShape.getSize().x) < 0;
}

// --- BIRD ---
Bird::Bird(float airHeight, float startX, const sf::Texture& tex, bool isValidTex) {
    startY = airHeight;
    timeAlive = 0.f;
    fallbackShape.setSize(sf::Vector2f(40.f, 40.f));
    fallbackShape.setFillColor(sf::Color::Yellow);
    fallbackShape.setPosition(startX, airHeight);
    hasTexture = isValidTex;
    if (hasTexture) {
        sprite.setTexture(tex);
        sprite.setScale(60.f / tex.getSize().x, 50.f / tex.getSize().y);
        sprite.setPosition(startX, airHeight - sprite.getGlobalBounds().height);
    }
}
Bird::~Bird() {}
void Bird::update(float deltaTime) {
    timeAlive += deltaTime;
    float wave = std::sin(timeAlive * 5.f) * 100.f * deltaTime;
    if (hasTexture) sprite.move(-speed * 1.5f * deltaTime, wave);
    fallbackShape.move(-speed * 1.5f * deltaTime, wave);
}
void Bird::draw(sf::RenderWindow& window) {
    if (hasTexture) window.draw(sprite); else window.draw(fallbackShape);
}
sf::FloatRect Bird::getBounds() const {
    if (hasTexture) return sprite.getGlobalBounds();
    return fallbackShape.getGlobalBounds();
}
bool Bird::isOffScreen() const {
    return (hasTexture ? sprite.getPosition().x + sprite.getGlobalBounds().width : fallbackShape.getPosition().x + fallbackShape.getSize().x) < 0;
}
