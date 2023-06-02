#pragma once

class PlayerCharacter;

class Item {
private:

    sf::Sprite sprite;
    sf::Texture texture;

public:

    virtual ~Item() {}

    Item(std::string texturePath)
    {
        texture.loadFromFile(texturePath);
        sprite.setTexture(texture);

        sf::Vector2f spriteSize(texture.getSize().x, texture.getSize().y);
        sprite.setOrigin(sf::Vector2f(spriteSize.x * 0.5f, spriteSize.y));
    }

    void setPosition(sf::Vector2f position) { sprite.setPosition(position); }

    sf::Sprite& getSprite() { return sprite; }

    sf::FloatRect getBounds() { return sprite.getGlobalBounds(); }
};

class HealingPotion : public Item {
public:

    HealingPotion() : Item(healPotionTexture) {}
};

class SpeedPotion : public Item {
public:

    SpeedPotion() : Item(speedPotionTexture) {}
};

class InvincibilityPotion : public Item {
public:

    InvincibilityPotion() : Item(invincibilityPotionTexture) {}
};

class ItemContainer {
private:

    std::vector<Item*> items;

public:

    ~ItemContainer()
    {
        reset();
    }

    void reset() 
    {
        for (auto item : items) 
        {
            delete item;
        }
        items.clear();
    }

    void update(PlayerCharacter* player);

    void addItem(Item* item) { items.push_back(item); }

    std::vector<sf::Sprite> getSprites()
    {
        std::vector<sf::Sprite> sprites;
        for (auto item : items) 
        {
            sprites.push_back(item->getSprite());
        }
        return sprites;
    }
};