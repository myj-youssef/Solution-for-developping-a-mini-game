#include "../include/Collectible.hpp"

Collectible::Collectible(float height, float startX, const sf::Texture& tex, bool isValidTex) : speed(350.f), collected(false) {
    fallbackShape.setRadius(15.f);
    fallbackShape.setFillColor(sf::Color::Yellow);
    fallbackShape.setPosition(startX, height);
    
    hasTexture = isValidTex;
    if (hasTexture) {
        sprite.setTexture(tex);
        sprite.setPosition(startX, height);
        float texWidth = tex.getSize().x;
        sprite.setScale(30.f / texWidth, 30.f / texWidth);
    }
}

Collectible::~Collectible() {}

void Collectible::update(float deltaTime) {
    if (hasTexture) sprite.move(-speed * deltaTime, 0.f);
    fallbackShape.move(-speed * deltaTime, 0.f);
}

void Collectible::draw(sf::RenderWindow& window) {
    if (collected) return;
    if (hasTexture) window.draw(sprite);
    else window.draw(fallbackShape);
}

sf::FloatRect Collectible::getBounds() const {
    if (hasTexture) return sprite.getGlobalBounds();
    return fallbackShape.getGlobalBounds();
}

bool Collectible::isOffScreen() const {
    float x = hasTexture ? sprite.getPosition().x + sprite.getGlobalBounds().width : fallbackShape.getPosition().x + fallbackShape.getRadius() * 2;
    return x < 0;
}
