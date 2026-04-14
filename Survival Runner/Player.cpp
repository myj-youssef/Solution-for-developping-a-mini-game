#include "Player.hpp"

Player::Player(float startX, float groundY, const sf::Texture& t1, const sf::Texture& t2, bool tLoaded)
    : groundLevel(groundY), gravity(2000.0f), jumpForce(800.0f), moveSpeed(250.0f),
      normalSize(50.0f, 100.0f), duckSize(50.0f, 50.0f),
      isJumping(false), isDucking(false),
      hasTextures(tLoaded), texRun1(&t1), texRun2(&t2), animTimer(0.0f) {
      
    if (hasTextures) {
        sprite.setTexture(*texRun1);
        sf::Vector2u texSize = texRun1->getSize();
        
        // NORMALISATION DE LA TAILLE DE L'IMAGE
        if (texSize.x > 0 && texSize.y > 0) {
            sprite.setScale(normalSize.x / static_cast<float>(texSize.x), 
                            normalSize.y / static_cast<float>(texSize.y));
            sprite.setOrigin(0.0f, static_cast<float>(texSize.y));
        }
        sprite.setPosition(startX, groundLevel);
    }
    
    fallbackShape.setSize(normalSize);
    fallbackShape.setFillColor(sf::Color::Blue);
    fallbackShape.setOrigin(0.0f, normalSize.y);
    fallbackShape.setPosition(startX, groundLevel);
    
    velocity = sf::Vector2f(0.0f, 0.0f);
}

void Player::update(float deltaTime, bool isRunningKey) {
    velocity.y += gravity * deltaTime;
    
    if (hasTextures) sprite.move(velocity.x * deltaTime, velocity.y * deltaTime);
    fallbackShape.move(velocity.x * deltaTime, velocity.y * deltaTime);
    
    float currentY = hasTextures ? sprite.getPosition().y : fallbackShape.getPosition().y;
    if (currentY >= groundLevel) {
        if (hasTextures) {
            sf::Vector2f pos = sprite.getPosition();
            pos.y = groundLevel;
            sprite.setPosition(pos);
        }
        sf::Vector2f posF = fallbackShape.getPosition();
        posF.y = groundLevel;
        fallbackShape.setPosition(posF);
        
        velocity.y = 0.0f;
        isJumping = false;
    }

    sf::Vector2f currentPos = hasTextures ? sprite.getPosition() : fallbackShape.getPosition();
    float width = hasTextures ? sprite.getGlobalBounds().width : fallbackShape.getSize().x;
    
    if (currentPos.x < 0.0f) {
        if (hasTextures) sprite.setPosition(0.0f, currentPos.y);
        fallbackShape.setPosition(0.0f, currentPos.y);
    } else if (currentPos.x > 800.0f - width) {
        if (hasTextures) sprite.setPosition(800.0f - width, currentPos.y);
        fallbackShape.setPosition(800.0f - width, currentPos.y);
    }

    if (hasTextures && isRunningKey && !isJumping && !isDucking) {
        animTimer += deltaTime;
        if (animTimer >= 0.15f) { 
            animTimer = 0.0f;
            if (sprite.getTexture() == texRun1) sprite.setTexture(*texRun2);
            else sprite.setTexture(*texRun1);
        }
    } else if (hasTextures && (!isRunningKey || isJumping || isDucking)) {
        sprite.setTexture(*texRun1);
        animTimer = 0.0f;
    }
}

void Player::draw(sf::RenderWindow& window) {
    if (hasTextures) window.draw(sprite);
    else window.draw(fallbackShape);
}

void Player::run(float directionMultiplier) {
    velocity.x = moveSpeed * directionMultiplier;
}

void Player::jump() {
    if (!isJumping) {
        velocity.y = -jumpForce;
        isJumping = true;
        if (isDucking) standUp();
    }
}

void Player::duck() {
    if (!isJumping && !isDucking) {
        isDucking = true;
        fallbackShape.setSize(duckSize);
        fallbackShape.setOrigin(0.0f, duckSize.y);
        
        if (hasTextures) {
            sf::Vector2u texSize = texRun1->getSize();
            if(texSize.x > 0 && texSize.y > 0) {
                sprite.setScale(normalSize.x / static_cast<float>(texSize.x), 
                               (normalSize.y / static_cast<float>(texSize.y)) * 0.5f);
            }
        }
    }
}

void Player::standUp() {
    if (isDucking) {
        isDucking = false;
        fallbackShape.setSize(normalSize);
        fallbackShape.setOrigin(0.0f, normalSize.y);
        
        if (hasTextures) {
            sf::Vector2u texSize = texRun1->getSize();
            if(texSize.x > 0 && texSize.y > 0) {
                sprite.setScale(normalSize.x / static_cast<float>(texSize.x), 
                                normalSize.y / static_cast<float>(texSize.y));
            }
        }
    }
}

void Player::reset() {
    if (hasTextures) sprite.setPosition(100.0f, groundLevel);
    fallbackShape.setPosition(100.0f, groundLevel); 
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    isJumping = false;
    standUp();
}

/**
 * @brief Récupère une Hitbox plus indulgente en réduisant le contour complet.
 */
sf::FloatRect Player::getHitbox() const {
    sf::FloatRect bounds = hasTextures ? sprite.getGlobalBounds() : fallbackShape.getGlobalBounds();
    // Retrait de 10% sur X et Y
    float shrinkX = bounds.width * 0.1f;
    float shrinkY = bounds.height * 0.1f;
    return sf::FloatRect(bounds.left + shrinkX / 2.0f, 
                         bounds.top + shrinkY / 2.0f, 
                         bounds.width - shrinkX, 
                         bounds.height - shrinkY);
}
