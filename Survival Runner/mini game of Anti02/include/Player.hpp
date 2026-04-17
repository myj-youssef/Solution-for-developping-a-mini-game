#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Platform;

/**
 * @brief Classe Player encapsulant la logique et la physique de l'avatar.
 * Elle respecte l'encapsulation (attributs privés) et l'optimisation mémoire
 * en utilisant des références de Texture transmises sans duplication de données.
 */
class Player {
private:
    float velocityY;
    float gravity;
    float jumpForce;
    float speedX;
    float groundHeight;
    bool isJumping;
    bool isDucking;
    
    int maxLives;
    int currentLives;
    float invincibilityTimer;
    
    // Sécurité Fallback si les assets sont absents
    sf::RectangleShape fallbackShape;
    
    // Le Sprite contient en interne un pointeur vers la Texture (Pas de dupliquata)
    sf::Sprite sprite;
    
    // Pointeurs stockés pour permettre l'animation dynamique de la course
    const sf::Texture* pTexRun1;
    const sf::Texture* pTexRun2;
    const sf::Texture* pTexDuck;

    bool hasTexRun1;
    bool hasTexRun2;
    bool hasTexDuck;

    float animTimer;
    int currentFrame;
    float normalWidth;
    float normalHeight;

public:
    Player(); // Constructeur vide (Permet l'instanciation de l'attribut dans Game)
    
    // Constructeur validé recevant const sf::Texture&
    Player(const sf::Texture& texR1, bool hasT1, const sf::Texture& texR2, bool hasT2, const sf::Texture& texD, bool hasTD);
    ~Player();

    void update(float deltaTime, const std::vector<Platform*>& platforms);
    void draw(sf::RenderWindow& window);
    
    void jump();
    void duck();
    void standUp();

    bool takeDamage();
    int getLives() const { return currentLives; }

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
};

#endif // PLAYER_HPP
