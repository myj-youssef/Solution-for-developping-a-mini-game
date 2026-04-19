#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <SFML/Graphics.hpp>

class Platform {
private:
    float speed;
    sf::Sprite sprite;
    sf::RectangleShape fallbackShape;
    bool hasTexture;
    
public:
    Platform(float height, float startX, float length, const sf::Texture& tex, bool isValidTex);
    ~Platform();

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    bool isOffScreen() const;
};

#endif // PLATFORM_HPP
