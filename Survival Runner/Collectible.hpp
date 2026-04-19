#ifndef COLLECTIBLE_HPP
#define COLLECTIBLE_HPP

#include <SFML/Graphics.hpp>

class Collectible {
private:
    float speed;
    sf::Sprite sprite;
    sf::CircleShape fallbackShape;
    bool hasTexture;
    bool collected;
    
public:
    Collectible(float height, float startX, const sf::Texture& tex, bool isValidTex);
    ~Collectible();

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    bool isOffScreen() const;
    
    bool isCollected() const { return collected; }
    void collect() { collected = true; }
};

#endif // COLLECTIBLE_HPP
