#include "Platform.hpp"

Platform::Platform(float height, float startX, float length, const sf::Texture& tex, bool isValidTex) : speed(350.f) {
    fallbackShape.setSize(sf::Vector2f(length, 40.f));
    fallbackShape.setFillColor(sf::Color(139, 69, 19)); // Brown
    fallbackShape.setPosition(startX, height);
    
    hasTexture = isValidTex;
    if (hasTexture) {
        sprite.setTexture(tex);
        sprite.setPosition(startX, height);
        float texWidth = tex.getSize().x;
        float texHeight = tex.getSize().y;
        sprite.setScale(length / texWidth, 40.f / texHeight);
    }
}

Platform::~Platform() {}

void Platform::update(float deltaTime) {
    if (hasTexture) sprite.move(-speed * deltaTime, 0.f);
    fallbackShape.move(-speed * deltaTime, 0.f);
}

void Platform::draw(sf::RenderWindow& window) {
    if (hasTexture) window.draw(sprite);
    else window.draw(fallbackShape);
}

sf::FloatRect Platform::getBounds() const {
    if (hasTexture) return sprite.getGlobalBounds();
    return fallbackShape.getGlobalBounds();
}

bool Platform::isOffScreen() const {
    float x = hasTexture ? sprite.getPosition().x + sprite.getGlobalBounds().width : fallbackShape.getPosition().x + fallbackShape.getSize().x;
    return x < 0;
}
