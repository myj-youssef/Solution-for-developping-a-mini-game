#include "Obstacle.hpp"

// ---------------------- MINE ----------------------

Mine::Mine(float startX, float startY, const sf::Texture& texture, bool textureLoaded) 
    : speed(300.0f), hasTexture(textureLoaded) {
    
    if (hasTexture) {
        sprite.setTexture(texture);
        sf::Vector2u texSize = texture.getSize();
        
        // NORMALISATION DE LA TAILLE (Boîte fixe virtuelle 40x40 pour garantir la fair-play)
        if (texSize.x > 0 && texSize.y > 0) {
            sprite.setScale(40.0f / static_cast<float>(texSize.x), 40.0f / static_cast<float>(texSize.y));
            sprite.setOrigin(0.0f, static_cast<float>(texSize.y));
        }
        sprite.setPosition(startX, startY);
    }
    
    fallbackShape.setSize(sf::Vector2f(40.0f, 40.0f));
    fallbackShape.setFillColor(sf::Color::Red);
    fallbackShape.setOrigin(0.0f, 40.0f);
    fallbackShape.setPosition(startX, startY);
}

void Mine::update(float deltaTime) {
    if (hasTexture) sprite.move(-speed * deltaTime, 0.0f);
    fallbackShape.move(-speed * deltaTime, 0.0f);
}

void Mine::draw(sf::RenderWindow& window) {
    if (hasTexture) window.draw(sprite);
    else window.draw(fallbackShape);
}

sf::FloatRect Mine::getHitbox() const {
    sf::FloatRect bounds = hasTexture ? sprite.getGlobalBounds() : fallbackShape.getGlobalBounds();
    float shrinkX = bounds.width * 0.1f;
    float shrinkY = bounds.height * 0.1f;
    return sf::FloatRect(bounds.left + shrinkX / 2.0f, bounds.top + shrinkY / 2.0f, bounds.width - shrinkX, bounds.height - shrinkY);
}

bool Mine::isOffScreen() const {
    if (hasTexture) return sprite.getPosition().x + sprite.getGlobalBounds().width < 0.0f;
    return fallbackShape.getPosition().x + fallbackShape.getSize().x < 0.0f;
}

// ---------------------- DRONE ----------------------

Drone::Drone(float startX, float startY, const sf::Texture& texture, bool textureLoaded) 
    : speed(400.0f), hasTexture(textureLoaded) {
        
    if (hasTexture) {
        sprite.setTexture(texture);
        sf::Vector2u texSize = texture.getSize();
        
        // NORMALISATION DE LA TAILLE (Boîte fixe virtuelle diametre 40)
        if (texSize.x > 0 && texSize.y > 0) {
            sprite.setScale(40.0f / static_cast<float>(texSize.x), 40.0f / static_cast<float>(texSize.y));
            sprite.setOrigin(static_cast<float>(texSize.x) / 2.0f, static_cast<float>(texSize.y) / 2.0f);
        }
        sprite.setPosition(startX, startY);
    }
    
    fallbackShape.setRadius(20.0f);
    fallbackShape.setFillColor(sf::Color(255, 165, 0)); 
    fallbackShape.setOrigin(20.0f, 20.0f);
    fallbackShape.setPosition(startX, startY);
}

void Drone::update(float deltaTime) {
    if (hasTexture) sprite.move(-speed * deltaTime, 0.0f);
    fallbackShape.move(-speed * deltaTime, 0.0f);
}

void Drone::draw(sf::RenderWindow& window) {
    if (hasTexture) window.draw(sprite);
    else window.draw(fallbackShape);
}

sf::FloatRect Drone::getHitbox() const {
    sf::FloatRect bounds = hasTexture ? sprite.getGlobalBounds() : fallbackShape.getGlobalBounds();
    // Le drone circulaire peut avoir une hitbox plus permissive pour compenser le carré englobant
    float shrinkX = bounds.width * 0.15f; 
    float shrinkY = bounds.height * 0.15f;
    return sf::FloatRect(bounds.left + shrinkX / 2.0f, bounds.top + shrinkY / 2.0f, bounds.width - shrinkX, bounds.height - shrinkY);
}

bool Drone::isOffScreen() const {
    if (hasTexture) return sprite.getPosition().x + sprite.getGlobalBounds().width / 2.0f < 0.0f;
    return fallbackShape.getPosition().x + fallbackShape.getRadius() < 0.0f;
}
