#pragma once

class Healthbar : public sf::Drawable, public sf::Transformable
{
private:

    sf::VertexArray m_vertices;
    sf::Texture m_healthbar;

    unsigned int maxHearts;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_healthbar;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

public:

    Healthbar() : maxHearts(0) {}

    bool load(const std::string& healthbar, unsigned int _maxHitPoints)
    {
        if (!m_healthbar.loadFromFile(healthbar)) return false;

        maxHearts = _maxHitPoints / 2;
        m_vertices.setPrimitiveType(sf::Quads);
        m_vertices.resize(maxHearts * 4);

        return true;
    }

    void resize(unsigned int _maxHitPoints)
    {
        maxHearts = _maxHitPoints / 2;
        m_vertices.setPrimitiveType(sf::Quads);
        m_vertices.resize(maxHearts * 4);
    }

    void update(const sf::Vector2f& starting_position, unsigned int _currentHitPoints)
    {
        unsigned int fullHearts = _currentHitPoints / 2;
        unsigned int halfHeart = _currentHitPoints % 2 == 0 ? 0 : 1;

        for (unsigned int i = 0; i < maxHearts; i++)
        {
            sf::Vertex* quad = &m_vertices[i * 4];

            quad[0].position = sf::Vector2f(starting_position.x + (i * tileSize.x), starting_position.y);
            quad[1].position = sf::Vector2f(starting_position.x + ((i + 1) * tileSize.x), starting_position.y);
            quad[2].position = sf::Vector2f(starting_position.x + ((i + 1) * tileSize.x), starting_position.y + tileSize.y);
            quad[3].position = sf::Vector2f(starting_position.x + (i * tileSize.x), starting_position.y + tileSize.y);

            if (i < fullHearts) {
                quad[0].texCoords = sf::Vector2f(0, 0);
                quad[1].texCoords = sf::Vector2f(16, 0);
                quad[2].texCoords = sf::Vector2f(16, 16);
                quad[3].texCoords = sf::Vector2f(0, 16);
            }
            else if (i < fullHearts + halfHeart) {
                quad[0].texCoords = sf::Vector2f(16, 0);
                quad[1].texCoords = sf::Vector2f(32, 0);
                quad[2].texCoords = sf::Vector2f(32, 16);
                quad[3].texCoords = sf::Vector2f(16, 16);
            }
            else {
                quad[0].texCoords = sf::Vector2f(32, 0);
                quad[1].texCoords = sf::Vector2f(48, 0);
                quad[2].texCoords = sf::Vector2f(48, 16);
                quad[3].texCoords = sf::Vector2f(32, 16);
            }
        }
    }
};

class Character {
protected:

    Animation idle_animation;
    Animation run_animation;
    Animation* current_animation;

    sf::Sprite sprite;

    bool isRunning;
    float movement_spd;

    unsigned int maxHitPoints;
    int currentHitPoints;

    void playIdleAnimation()
    {
        // Update current animation to idle animation
        if (current_animation != &idle_animation) current_animation->stop();
        current_animation = &idle_animation;
        current_animation->play();
    }

    void playRunAnimation()
    {
        // Update current animation to run animation
        if (current_animation != &run_animation) current_animation->stop();
        current_animation = &run_animation;
        current_animation->play();
    }

    void move(sf::Vector2f movementVector, const float& deltaTime)
    {
        if (movementVector.x != 0.f && movementVector.y != 0.f)
        {
            // Calculate the length of the vector
            float length = std::sqrt(movementVector.x * movementVector.x + movementVector.y * movementVector.y);

            // Divide the vector by its length to normalize it
            movementVector /= length;
        }

        if (movementVector.x != 0 || movementVector.y != 0) isRunning = true;
        else isRunning = false;

        if (movementVector.x > 0) sprite.setScale(1.f, 1.f);
        else if (movementVector.x < 0) sprite.setScale(-1.f, 1.f);

        movementVector *= movement_spd;
        sprite.move(movementVector * movement_spd * deltaTime);
    }

    void handleAnimations() 
    {
        if (isRunning) playRunAnimation();
        else playIdleAnimation();
    }

public:

    Character(std::string _idleAnim, std::string _runAnim, float _movement_spd, unsigned int _maxHitPoints) 
        : idle_animation(0.1f, 4, true), run_animation(0.1f, 4, true), movement_spd(_movement_spd), maxHitPoints(_maxHitPoints), currentHitPoints(_maxHitPoints), isRunning(false)
    {
        // Load textures for idle animation
        idle_animation.load(_idleAnim);

        // Load textures for run animation
        run_animation.load(_runAnim);

        // Start with idle animation
        current_animation = &idle_animation;
        current_animation->play();

        // Set sprite's origin to it's center on x axis
        sf::Texture texture = current_animation->getCurrentFrame();
        sf::Vector2f spriteSize(texture.getSize().x, texture.getSize().y);
        sprite.setOrigin(sf::Vector2f(spriteSize.x * 0.5f, spriteSize.y));
    }

    virtual void takeDamage(unsigned int damage) { currentHitPoints -= damage; }

    int getCurrentHP() const { return currentHitPoints; }

    int getMaxHP() const { return maxHitPoints; }

    sf::Vector2f getPosition() const { return sprite.getPosition(); }

    sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }

    void setPosition(const sf::Vector2f& position) { sprite.setPosition(position); }

    sf::Sprite& getSprite() { return sprite; }
};

class PlayerCharacter : public Character {
protected:

    Weapon* currentWeapon;

    Healthbar healthbar;

    sf::Clock attackClock;
    sf::Time attackCooldown;

    sf::Clock interactionClock;
    sf::Time interactionCooldown = sf::seconds(0.5);

    sf::Clock potionUseClock;
    sf::Time potionUseCooldown = sf::seconds(0.5);

    sf::Clock speedEffectClock;
    sf::Time speedEffectTime = sf::seconds(3);

    sf::Clock invincibilityEffectClock;
    sf::Time invincibilityEffectTime = sf::seconds(3);

    sf::Clock playerImmunityClock;
    sf::Time playerImmunityTime = sf::seconds(3);

    unsigned int healingPotions = 0;
    unsigned int speedPotions = 0;
    unsigned int invincibilityPotions = 0;

    float boostedMvSpeed;
    float normalMvSpeed;

    bool potionUsed = false;

    void useHealingPotion()
    {
        if (healingPotions > 0 && currentHitPoints < maxHitPoints && potionUseClock.getElapsedTime() >= potionUseCooldown) {
            currentHitPoints++;
            healingPotions--;
            potionUseClock.restart();
        }
    }

    void useSpeedPotion() 
    {
        if (speedPotions > 0 && potionUseClock.getElapsedTime() >= potionUseCooldown) {
            potionUsed = true;
            movement_spd = boostedMvSpeed;
            speedPotions--;
            potionUseClock.restart();
            speedEffectClock.restart();
        }
    }

    void useInvincibilityPotion()
    {
        if (invincibilityPotions > 0 && potionUseClock.getElapsedTime() >= potionUseCooldown) {
            potionUsed = true;
            invincibilityPotions--;
            potionUseClock.restart();
            invincibilityEffectClock.restart();
        }
    }

    void getInputs(const float& deltaTime) 
    {
        // Get inputs for movement
        sf::Vector2f movement = sf::Vector2f(0.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            movement.x -= 1.f;
            currentWeapon->setTargetRotation(-80.f);
            currentWeapon->setScale(sf::Vector2f(-1, 1));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            movement.x += 1.f;
            currentWeapon->setTargetRotation(80.f);
            currentWeapon->setScale(sf::Vector2f(1, 1));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += 1.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) useHealingPotion();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) useSpeedPotion();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) useInvincibilityPotion();

        currentWeapon->setPosition(sf::Vector2f(sprite.getPosition().x, sprite.getPosition().y - 3.f));

        if (speedEffectClock.getElapsedTime() > speedEffectTime) {
            movement_spd = normalMvSpeed;
        }
        else if (potionUsed) {
            movement_spd = boostedMvSpeed;
        }
        move(movement, deltaTime);
    }

public:
    PlayerCharacter(std::string _idleAnim, std::string _runAnim, float _movement_spd, int _maxHitPoints) 
        : Character(_idleAnim, _runAnim, _movement_spd, _maxHitPoints), currentWeapon(nullptr), attackCooldown(sf::seconds(0))
    {
        healthbar.load(healthbarTexture, _maxHitPoints);
        boostedMvSpeed = movement_spd + 1.5f;
        normalMvSpeed = movement_spd;
    }

    void update(const float& deltaTime, const sf::Vector2f viewportPosition)
    {
        isRunning = false;

        getInputs(deltaTime);
        handleAnimations();
        healthbar.update(viewportPosition, currentHitPoints);

        // Set sprite texture based on animation currently playing
        current_animation->update(deltaTime);
        sprite.setTexture(current_animation->getCurrentFrame());

        currentWeapon->playAttackAnimation(deltaTime);
    }

    virtual void takeDamage(unsigned int damage) 
    { 
        if (invincibilityEffectClock.getElapsedTime() > invincibilityEffectTime && playerImmunityClock.getElapsedTime() > playerImmunityTime) {
            currentHitPoints -= damage;
            playerImmunityClock.restart();
        }
    }

    Weapon* equipWeapon(Weapon* weapon) 
    { 
        Weapon* ptr = currentWeapon;
        currentWeapon = weapon; 
        attackCooldown = sf::seconds(currentWeapon->getAttackCooldown());
        currentWeapon->setPosition(sf::Vector2f(sprite.getPosition().x, sprite.getPosition().y - 3.f));
        return ptr;
    }

    bool canAttack(const float& dt)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (attackClock.getElapsedTime() >= attackCooldown) {
                attackClock.restart();
                currentWeapon->resetAnim();
                return true;
            }
        }
        return false;
    }

    bool interact()
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            if (interactionClock.getElapsedTime() >= interactionCooldown) {
                return true;
            }
        }
        return false;
    }

    void restartInteractClock() { interactionClock.restart(); }

    sf::Sprite& getWeaponSprite() { return currentWeapon->getSprite(); }

    sf::FloatRect getWeaponHitbox()
    {
        if (currentWeapon->getTargetRotation() > 0) {
            sf::FloatRect defaultHitbox(sf::Vector2f(getPosition().x - 5.f, getPosition().y - currentWeapon->getWeaponLength()), sf::Vector2f(currentWeapon->getWeaponLength() + 5.f, currentWeapon->getWeaponLength()));
            return defaultHitbox;
        }
        sf::FloatRect defaultHitbox(sf::Vector2f(getPosition().x - currentWeapon->getWeaponLength(), getPosition().y - currentWeapon->getWeaponLength()), sf::Vector2f(currentWeapon->getWeaponLength() + 5.f, currentWeapon->getWeaponLength()));
        return defaultHitbox;
    }
    
    Healthbar& getHealthbar() { return healthbar; }

    unsigned int getWeaponDamage() const { return currentWeapon->getDamage(); }

    sf::FloatRect getHitbox() const 
    { 
        sf::FloatRect defaultBounds = getGlobalBounds();
        return sf::FloatRect(defaultBounds.left, defaultBounds.top + defaultBounds.height / 2, defaultBounds.width, defaultBounds.height / 2);
    }

    void addItem(Item* item)
    {
        if (HealingPotion* derived = dynamic_cast<HealingPotion*>(item)) {
            healingPotions++;
        }
        else if (SpeedPotion* derived = dynamic_cast<SpeedPotion*>(item)) {
            speedPotions++;
        }
        else if (InvincibilityPotion* derived = dynamic_cast<InvincibilityPotion*>(item)) {
            invincibilityPotions++;
        }
    }

    unsigned int getHealingPotions() const { return healingPotions; }
    unsigned int getSpeedPotions() const { return speedPotions; }
    unsigned int getInvinPotions() const { return invincibilityPotions; }
};

void WeaponContainer::update(PlayerCharacter* player) {
    if (player->interact()) {
        for (auto it = activeWeapons.begin(); it != activeWeapons.end(); ++it) 
        {
            Weapon* weapon = *it;
            if (weapon->getSprite().getGlobalBounds().intersects(player->getGlobalBounds())) 
            {
                activeWeapons.erase(it);
                activeWeapons.push_back(player->equipWeapon(weapon));
                player->restartInteractClock();
                return;
            }
        }
    }
}

void ItemContainer::update(PlayerCharacter* player) {
    if (player->interact()) {
        for (auto it = items.begin(); it != items.end(); ++it)
        {
            auto item = *it;
            if (item->getSprite().getGlobalBounds().intersects(player->getGlobalBounds()))
            {
                items.erase(it);
                player->addItem(item);
                player->restartInteractClock();
                return;
            }
        }
    }
}

class EnemyCharacter : public Character {
protected:

    sf::Clock moveClock;
    sf::Time moveTime;
    sf::Time idleTime;

    unsigned int damage;
    float playerDetectRange = 7;

    sf::RectangleShape healthbar;

    float healthbarSize;
    float healthbarYOffset;

    void calculateMoveDirection(const sf::Vector2f& playerPosition, const float& dt) 
    {
        sf::Vector2f position = getPosition();
        sf::Vector2f direction = playerPosition - position;

        float distance = std::sqrt(pow(position.x - playerPosition.x, 2) + pow(position.y - playerPosition.y, 2));

        if ((distance / tileSize.x) <= playerDetectRange) move(direction, dt);
    }

    bool canMove() 
    { 
        if (moveClock.getElapsedTime() <= moveTime) {
            return true;
        }
        else if (moveClock.getElapsedTime() >= moveTime + idleTime) {
            moveClock.restart();
            return true;
        }
        return false;
    }

    bool playerDetected(const sf::FloatRect& playerBounds) { return playerBounds.intersects(getGlobalBounds()); }

public:
    EnemyCharacter(std::string _idleAnim, std::string _runAnim, float _movement_spd, int _hitPoints, float _healthbarSize = 12.f, float _healthbarYOffset = 18.f)
        : Character(_idleAnim, _runAnim, _movement_spd, _hitPoints), damage(1), healthbarSize(_healthbarSize), healthbarYOffset(_healthbarYOffset)
    {
        int move_time = getRandomInRange(5, 20);
        int idle_time = getRandomInRange(0, 5);

        moveClock.restart();

        moveTime = sf::seconds(move_time);
        idleTime = sf::seconds(idle_time);

        healthbar = sf::RectangleShape(sf::Vector2f(healthbarSize, 1));
        healthbar.setFillColor(sf::Color::Red);
    }

    void update(const float& deltaTime, PlayerCharacter* player)
    {
        isRunning = false;

        if (canMove()) {
            calculateMoveDirection(player->getPosition(), deltaTime);
        }

        if (playerDetected(player->getHitbox())) {
            player->takeDamage(damage);
        }

        handleAnimations();

        // Set sprite texture based on animation currently playing
        current_animation->update(deltaTime);
        sprite.setTexture(current_animation->getCurrentFrame());

        float healthPercentage = (float)currentHitPoints / (float)maxHitPoints;
        healthbar.setSize(sf::Vector2f(healthbarSize * healthPercentage, 1));
        healthbar.setPosition(sf::Vector2f(getPosition().x - healthbarSize / 2, getPosition().y - healthbarYOffset));
    }

    sf::RectangleShape& getHealthbar() { return healthbar; }
};