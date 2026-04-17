#include "../include/Player.hpp"
#include "../include/Platform.hpp"
#include <iostream>
#include <cmath>

Player::Player() {
    pTexRun1 = nullptr;
    pTexRun2 = nullptr;
    pTexDuck = nullptr;
    hasTexRun1 = false;
    hasTexRun2 = false;
    hasTexDuck = false;
}

/**
 * @brief Constructeur - Utilisation de références vers textures (const sf::Texture&)
 * Optimise l'empreinte mémoire et respecte les consignes du Chapitre 07 sur 
 * la gestion optimisée des ressources externes.
 */
Player::Player(const sf::Texture& texR1, bool hasT1, const sf::Texture& texR2, bool hasT2, const sf::Texture& texD, bool hasTD) {
    normalWidth = 60.f;  // slightly wider to fit the sprite aspect ratio
    normalHeight = 80.f;
    groundHeight = 500.f;
    
    velocityY = 0.f;
    gravity = 1500.f;
    jumpForce = -700.f;
    speedX = 350.f;
    
    isJumping = false;
    isDucking = false;
    maxLives = 2;
    currentLives = 2;
    invincibilityTimer = 0.f;
    animTimer = 0.f;
    currentFrame = 1;

    // Fallback visuel
    fallbackShape.setSize(sf::Vector2f(normalWidth, normalHeight));
    fallbackShape.setFillColor(sf::Color::Green);
    fallbackShape.setPosition(100.f, groundHeight - normalHeight);

    // Enregistrement des pointeurs associés
    pTexRun1 = hasT1 ? &texR1 : nullptr;
    pTexRun2 = hasT2 ? &texR2 : nullptr;
    pTexDuck = hasTD ? &texD : nullptr;
    hasTexRun1 = hasT1;
    hasTexRun2 = hasT2;
    hasTexDuck = hasTD;

    if (hasTexRun1) {
        sprite.setTexture(*pTexRun1);
        float sX = normalWidth / pTexRun1->getSize().x;
        float sY = normalHeight / pTexRun1->getSize().y;
        sprite.setScale(sX, sY);
        sprite.setPosition(100.f, groundHeight - sprite.getGlobalBounds().height);
    }
}

Player::~Player() {}

void Player::update(float deltaTime, const std::vector<Platform*>& platforms) {
    if (invincibilityTimer > 0.f) {
        invincibilityTimer -= deltaTime;
        if (hasTexRun1 || hasTexDuck) {
            if (fmod(invincibilityTimer, 0.2f) < 0.1f) {
                sprite.setColor(sf::Color(255, 255, 255, 128));
            } else {
                sprite.setColor(sf::Color(255, 255, 255, 255));
            }
        }
    } else {
        if (hasTexRun1 || hasTexDuck) sprite.setColor(sf::Color(255, 255, 255, 255));
    }

    float currentX = getPosition().x;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && currentX + normalWidth < 800.f) {
        if (hasTexRun1 || hasTexDuck) sprite.move(speedX * deltaTime, 0);
        fallbackShape.move(speedX * deltaTime, 0);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && currentX > 0.f) {
        if (hasTexRun1 || hasTexDuck) sprite.move(-speedX * deltaTime, 0);
        fallbackShape.move(-speedX * deltaTime, 0);
    }

    if (isJumping) {
        velocityY += gravity * deltaTime;
        
        fallbackShape.move(0, velocityY * deltaTime);
        if (hasTexRun1) sprite.move(0, velocityY * deltaTime);
        
        float currentY = hasTexRun1 ? sprite.getPosition().y + sprite.getGlobalBounds().height : fallbackShape.getPosition().y + fallbackShape.getSize().y;
        
        float targetY = groundHeight;
        sf::FloatRect playerBounds = getBounds();
        
        if (velocityY > 0) { // falling down
            for (Platform* plat : platforms) {
                sf::FloatRect pBounds = plat->getBounds();
                if (playerBounds.left + playerBounds.width > pBounds.left && playerBounds.left < pBounds.left + pBounds.width) {
                    float playerPrevBottom = currentY - (velocityY * deltaTime);
                    if (playerPrevBottom <= pBounds.top + 15.f && currentY >= pBounds.top) {
                        targetY = pBounds.top;
                        break;
                    }
                }
            }
        }
        
        if (currentY >= targetY) {
            if (hasTexRun1) sprite.setPosition(sprite.getPosition().x, targetY - sprite.getGlobalBounds().height);
            fallbackShape.setPosition(fallbackShape.getPosition().x, targetY - fallbackShape.getSize().y);
            isJumping = false;
            velocityY = 0.f;
        }
    } else {
        float currentY = hasTexRun1 ? sprite.getPosition().y + sprite.getGlobalBounds().height : fallbackShape.getPosition().y + fallbackShape.getSize().y;
        if (currentY < groundHeight) {
            bool onPlatform = false;
            sf::FloatRect playerBounds = getBounds();
            for (Platform* plat : platforms) {
                sf::FloatRect pBounds = plat->getBounds();
                if (playerBounds.left + playerBounds.width > pBounds.left && playerBounds.left < pBounds.left + pBounds.width) {
                    if (std::abs(currentY - pBounds.top) < 5.f) {
                        onPlatform = true;
                        break;
                    }
                }
            }
            if (!onPlatform) {
                isJumping = true;
                velocityY = 0.f; // start falling
            }
        }

        if (!isDucking && (hasTexRun1 || hasTexRun2)) {
            // Animation du Sprite
            animTimer += deltaTime;
            if (animTimer > 0.15f) { // Cycle calibré validé
                animTimer = 0.f;
                currentFrame = (currentFrame == 1) ? 2 : 1;
                
                if (currentFrame == 1 && hasTexRun1) sprite.setTexture(*pTexRun1);
                else if (currentFrame == 2 && hasTexRun2) sprite.setTexture(*pTexRun2);
            }
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    if (hasTexRun1 || hasTexDuck) window.draw(sprite);
    else window.draw(fallbackShape);
}

void Player::jump() {
    if (!isJumping && !isDucking) {
        isJumping = true;
        velocityY = jumpForce;
    }
}

void Player::duck() {
    if (!isDucking) { // Retrait de "!isJumping" pour autoriser le duck aérien
        isDucking = true;
        
        // Sauvegarde la position du bas (les pieds) pour réaligner après le changement de taille
        float oldBottom = fallbackShape.getPosition().y + fallbackShape.getSize().y;
        if (hasTexRun1 || hasTexDuck) {
            oldBottom = sprite.getPosition().y + sprite.getGlobalBounds().height;
        }
        
        fallbackShape.setSize(sf::Vector2f(normalWidth, normalHeight / 2.f));
        fallbackShape.setFillColor(sf::Color(0, 200, 0)); 
        
        if (hasTexDuck) {
            sprite.setTexture(*pTexDuck);
            float sX = normalWidth / pTexDuck->getSize().x;
            float sY = (normalHeight / 2.f) / pTexDuck->getSize().y;
            sprite.setScale(sX, sY);
        } else if (hasTexRun1) {
            float sX = normalWidth / pTexRun1->getSize().x;
            float sY = (normalHeight / 2.f) / pTexRun1->getSize().y;
            sprite.setScale(sX, sY);
        }

        // Repositionne exactement à l'ancienne base
        fallbackShape.setPosition(fallbackShape.getPosition().x, oldBottom - fallbackShape.getSize().y);
        if (hasTexDuck || hasTexRun1) {
            sprite.setPosition(sprite.getPosition().x, oldBottom - sprite.getGlobalBounds().height);
        }

        // Fast-Fall : Si on est en l'air et qu'on monte, la touche Bas annule l'ascension !
        if (isJumping && velocityY < 0.f) {
            velocityY = 0.f; 
        }
    }
}

void Player::standUp() {
    if (isDucking) {
        isDucking = false;
        
        // Pour se relever sans traverser le sol, on s'aligne aussi par rapport au bas !
        float oldBottom = fallbackShape.getPosition().y + fallbackShape.getSize().y;
        if (hasTexRun1 || hasTexDuck) {
            oldBottom = sprite.getPosition().y + sprite.getGlobalBounds().height;
        }
        
        fallbackShape.setSize(sf::Vector2f(normalWidth, normalHeight));
        fallbackShape.setFillColor(sf::Color::Green);
        
        if (hasTexRun1) {
            float sX = normalWidth / pTexRun1->getSize().x;
            float sY = normalHeight / pTexRun1->getSize().y;
            sprite.setScale(sX, sY);
            sprite.setTexture(*pTexRun1);
        }

        // Réalignement par le bas
        fallbackShape.setPosition(fallbackShape.getPosition().x, oldBottom - fallbackShape.getSize().y);
        if (hasTexRun1) {
            sprite.setPosition(sprite.getPosition().x, oldBottom - sprite.getGlobalBounds().height);
        }
    }
}

sf::FloatRect Player::getBounds() const {
    if (hasTexRun1 || hasTexDuck) {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        bounds.left += bounds.width * 0.2f;
        bounds.width *= 0.6f;
        bounds.top += bounds.height * 0.1f;
        bounds.height *= 0.9f;
        return bounds;
    }
    return fallbackShape.getGlobalBounds();
}

sf::Vector2f Player::getPosition() const {
    return hasTexRun1 || hasTexDuck ? sprite.getPosition() : fallbackShape.getPosition();
}

bool Player::takeDamage() {
    if (invincibilityTimer > 0.f) return false;
    currentLives--;
    if (currentLives <= 0) return true;
    invincibilityTimer = 1.5f;
    return false;
}
