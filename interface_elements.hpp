#pragma once

class EndGameScreen {
private:

	sf::RenderWindow& window;
	sf::Font font;
	sf::Text text;
	sf::Text index;

public:

	EndGameScreen(sf::RenderWindow& window) : window(window)
	{
		if (!font.loadFromFile("./assets/fonts/font.ttf")) {
			// Handle font loading error
		}

		text.setFont(font);
		text.setCharacterSize(32);
		text.setScale(.2f, .2f);
		text.setString("You Lost\nPress [ENTER] to restart the game");

		index.setFont(font);
		index.setCharacterSize(24);
		index.setScale(.2f, .2f);
		index.setString("Author:\nka5ha 152082\n\nAssets:\n16x16 DungeonTileset II by 0x72 (Thank You :D)");
	}

	bool handleInput()
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
	}

	void setText(const std::string& _text)
	{
		text.setString(_text);
	}

	void render(sf::View& viewport)
	{
		sf::Vector2f viewportCenter = viewport.getCenter();
		sf::FloatRect textBounds = text.getLocalBounds();
		text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
		text.setPosition(viewportCenter.x, viewportCenter.y);

		index.setPosition(viewportCenter.x - viewport.getSize().x / 2 + 10.f, viewportCenter.y - viewport.getSize().y / 2 + 10.f);

		window.draw(text);
		window.draw(index);
	}
};

class PotionStatus {
private:

	sf::RenderWindow& window;

	sf::Font font;
	sf::Text healPotions;
	sf::Text speedPotions;
	sf::Text invinPotions;

	HealingPotion healTexture;
	SpeedPotion speedTexture;
	InvincibilityPotion invinTexture;

public:

	PotionStatus(sf::RenderWindow& window) : window(window)
	{
		if (!font.loadFromFile("./assets/fonts/font.ttf")) {
			// Handle font loading error
		}

		healPotions.setFont(font);
		healPotions.setCharacterSize(48);
		healPotions.setScale(.2f, .2f);

		invinPotions.setFont(font);
		invinPotions.setCharacterSize(48);
		invinPotions.setScale(.2f, .2f);

		speedPotions.setFont(font);
		speedPotions.setCharacterSize(48);
		speedPotions.setScale(.2f, .2f);
	}

	void render(sf::View& viewport, PlayerCharacter* player)
	{
		healPotions.setString(std::to_string(player->getHealingPotions()));
		speedPotions.setString(std::to_string(player->getSpeedPotions()));
		invinPotions.setString(std::to_string(player->getInvinPotions()));

		sf::Vector2f viewportCenter = viewport.getCenter();

		sf::FloatRect textBounds = healPotions.getLocalBounds();
		healPotions.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
		healPotions.setPosition(viewportCenter.x + viewport.getSize().x / 2 - 90.f, viewportCenter.y - viewport.getSize().y / 2 + 15.f);
		healTexture.setPosition(sf::Vector2f(healPotions.getPosition().x + 10.f, healPotions.getPosition().y + 6.f));

		textBounds = speedPotions.getLocalBounds();
		speedPotions.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
		speedPotions.setPosition(viewportCenter.x + viewport.getSize().x / 2 - 60.f, viewportCenter.y - viewport.getSize().y / 2 + 15.f);
		speedTexture.setPosition(sf::Vector2f(speedPotions.getPosition().x + 10.f, speedPotions.getPosition().y + 6.f));

		textBounds = invinPotions.getLocalBounds();
		invinPotions.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
		invinPotions.setPosition(viewportCenter.x + viewport.getSize().x / 2 - 30.f, viewportCenter.y - viewport.getSize().y / 2 + 15.f);
		invinTexture.setPosition(sf::Vector2f(invinPotions.getPosition().x + 10.f, invinPotions.getPosition().y + 6.f));

		window.draw(healPotions);
		window.draw(speedPotions);
		window.draw(invinPotions);

		window.draw(healTexture.getSprite());
		window.draw(speedTexture.getSprite());
		window.draw(invinTexture.getSprite());
	}
};