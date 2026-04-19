#include "Obstacle.hpp"

// --- OBSTACLE BASE ---
Obstacle::Obstacle() : speed(350.f), hasTexture(false), fallbackShape(nullptr) {}

Obstacle::~Obstacle() {
    if (fallbackShape != nullptr) {
        delete fallbackShape;
        fallbackShape = nullptr;
    }
}

void Obstacle::update(float deltaTime) {
    if (hasTexture) sprite.move(-speed * deltaTime, 0.f);
    if (fallbackShape) fallbackShape->move(-speed * deltaTime, 0.f);
}

void Obstacle::draw(sf::RenderWindow& window) {
    if (hasTexture) {
        window.draw(sprite);
    } else if (fallbackShape) {
        window.draw(*fallbackShape);
    }
}

sf::FloatRect Obstacle::getBounds() const {
    if (hasTexture) return sprite.getGlobalBounds();
    if (fallbackShape) return fallbackShape->getGlobalBounds();
    return sf::FloatRect();
}

bool Obstacle::isOffScreen() const {
    float x = 0;
    if (hasTexture) {
        x = sprite.getPosition().x + sprite.getGlobalBounds().width;
    } else if (fallbackShape) {
        x = fallbackShape->getPosition().x + fallbackShape->getGlobalBounds().width;
    }
    return x < 0;
}

// --- ENEMY ---
Enemy::Enemy(float groundHeight, float startX, const sf::Texture& tex, bool isValidTex) {
    sf::RectangleShape* shape = new sf::RectangleShape(sf::Vector2f(40.f, 30.f));
    shape->setFillColor(sf::Color::Red);
    shape->setPosition(startX, groundHeight - 30.f);
    fallbackShape = shape; // Polymorphic assignment
    
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

sf::FloatRect Enemy::getBounds() const {
    if (hasTexture) {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        bounds.left += bounds.width * 0.1f;
        bounds.width *= 0.8f;
        bounds.top += bounds.height * 0.2f;
        bounds.height *= 0.8f;
        return bounds;
    }
    return Obstacle::getBounds();
}

// --- TREASURE ---
Treasure::Treasure(float airHeight, float startX, const sf::Texture& tex, bool isValidTex) {
    speed = 350.f * 1.3f; // Faster speed
    sf::CircleShape* shape = new sf::CircleShape(20.f);
    shape->setFillColor(sf::Color(255, 165, 0));
    shape->setPosition(startX, airHeight);
    fallbackShape = shape; // Polymorphic assignment
    
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

sf::FloatRect Treasure::getBounds() const {
    if (hasTexture) {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        bounds.left += bounds.width * 0.2f;
        bounds.width *= 0.6f;
        bounds.top += bounds.height * 0.2f;
        bounds.height *= 0.6f;
        return bounds;
    }
    return Obstacle::getBounds();
}

// --- MINE ---
Mine::Mine(float groundHeight, float startX, const sf::Texture& tex, bool isValidTex) {
    sf::RectangleShape* shape = new sf::RectangleShape(sf::Vector2f(40.f, 40.f));
    shape->setFillColor(sf::Color(100, 100, 100));
    shape->setPosition(startX, groundHeight - 40.f);
    fallbackShape = shape; // Polymorphic assignment
    
    hasTexture = isValidTex;
    if (hasTexture) {
        sprite.setTexture(tex);
        sprite.setScale(50.f / tex.getSize().x, 50.f / tex.getSize().y);
        sprite.setPosition(startX, groundHeight - sprite.getGlobalBounds().height);
    }
}
Mine::~Mine() {}

// --- DRONE ---
Drone::Drone(float airHeight, float startX, const sf::Texture& tex, bool isValidTex) {
    speed = 350.f * 1.2f; // Faster drone speed modified safely in constructor
    
    sf::RectangleShape* shape = new sf::RectangleShape(sf::Vector2f(50.f, 30.f));
    shape->setFillColor(sf::Color::Cyan);
    shape->setPosition(startX, airHeight - 30.f);
    fallbackShape = shape; // Polymorphic assignment
    
    hasTexture = isValidTex;
    if (hasTexture) {
        sprite.setTexture(tex);
        sprite.setScale(70.f / tex.getSize().x, 40.f / tex.getSize().y);
        sprite.setPosition(startX, airHeight - sprite.getGlobalBounds().height);
    }
}
Drone::~Drone() {}

// --- BIRD ---
Bird::Bird(float airHeight, float startX, const sf::Texture& tex, bool isValidTex) {
    startY = airHeight;
    timeAlive = 0.f;
    
    sf::RectangleShape* shape = new sf::RectangleShape(sf::Vector2f(40.f, 40.f));
    shape->setFillColor(sf::Color::Yellow);
    shape->setPosition(startX, airHeight);
    fallbackShape = shape; // Polymorphic assignment
    
    hasTexture = isValidTex;
    if (hasTexture) {
        sprite.setTexture(tex);
        sprite.setScale(60.f / tex.getSize().x, 50.f / tex.getSize().y);
        sprite.setPosition(startX, airHeight - sprite.getGlobalBounds().height);
    }
}
Bird::~Bird() {}

void Bird::update(float deltaTime) {
    // Override spécifique : trajectoire Kamikaze vers le joueur
    float moveX = -speed * 1.5f * deltaTime;
    float moveY = speed * 1.5f * 0.466f * deltaTime;
    
    if (hasTexture) {
        sprite.move(moveX, moveY);
    }
    if (fallbackShape) {
        fallbackShape->move(moveX, moveY);
    }
}
