#pragma once

class Chest {
private:

	sf::Sprite sprite;

	Weapon* containedWeapon;
	bool isOpen;
	Animation openAnim;

public:

	~Chest()
	{
		delete containedWeapon;
	}

	Chest(WeaponContainer* weaponPool) : isOpen(false), openAnim(0.1f, 3, false)
	{
		openAnim.load(chestOpenAnim);

		int weaponIndex = getRandomInRange(0, weaponPool->getCurrentSize() - 1);

		containedWeapon = weaponPool->removeByIndex(weaponIndex);

		sprite.setTexture(openAnim.getCurrentFrame());
	}

	Weapon* open()
	{
		openAnim.play();
		isOpen = true;

		containedWeapon->setPosition(sf::Vector2f(sprite.getPosition().x + 20.f, sprite.getPosition().y + 20.f));
		Weapon* ptr = containedWeapon;
		containedWeapon = nullptr;

		return ptr;
	}

	void update(const float& dt)
	{
		openAnim.update(dt);
		sprite.setTexture(openAnim.getCurrentFrame());
	}

	void setPosition(const sf::Vector2f& position) { sprite.setPosition(position); }

	sf::Sprite& getSprite() { return sprite; }
	
	bool isChestOpen() { return isOpen; }
};

class ChestContainer {
private:

	std::vector<Chest*> chests;
	
	WeaponContainer* weaponPool;
	WeaponContainer* weaponsOnGround;

public:

	~ChestContainer()
	{
		reset();
	}

	ChestContainer(WeaponContainer* wP, WeaponContainer* wOG) : weaponPool(wP), weaponsOnGround(wOG) {}

	void reset() 
	{
		for (Chest* chest : chests) {
			delete chest;
		}
		chests.clear();
	}

	void spawnChests(const std::vector<Room*> chestRooms)
	{
		reset();

		for (const Room* room : chestRooms)
		{
			chests.push_back(new Chest(weaponPool));

			unsigned int x = (room->getX() + room->getWidth() / 2) * tileSize.x;
			unsigned int y = (room->getY() + room->getHeight() / 2) * tileSize.y;

			chests.back()->setPosition(sf::Vector2f(x, y));
		}
	}

	void update(const float& dt, PlayerCharacter* player)
	{
		for (Chest* chest : chests) {
			chest->update(dt);
		}

		if (player->interact())
		{
			for (Chest* chest : chests) 
			{
				if (chest->getSprite().getGlobalBounds().intersects(player->getGlobalBounds()) && !chest->isChestOpen())
				{
					weaponsOnGround->addWeapon(chest->open());
					player->restartInteractClock();
				}
			}
		}
	}

	std::vector<sf::Sprite> getChestSprites() 
	{
		std::vector<sf::Sprite> v;
		for (Chest* chest : chests) {
			v.push_back(chest->getSprite());
		}
		return v;
	}
};