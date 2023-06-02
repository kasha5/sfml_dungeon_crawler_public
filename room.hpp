#pragma once

class Room {
private:
	int width;
	int height;
	int startX = 100;
	int startY = 100;
	sf::Texture floorTexture;
	sf::Sprite floorSprite;

public:
	Room(int _width, int _height) : width(_width), height(_height) 
	{
		floorTexture.loadFromFile("./assets/room/floors/floor_1.png");
		floorSprite.setTexture(floorTexture);
		floorSprite.setScale(3.0f, 3.0f);
	}

	void draw(sf::RenderWindow& window) 
	{
		int numTilesX = width / 16;
		int numTilesY = height / 16;

		for (int y = 0; y < numTilesY; y++) {
			for (int x = 0; x < numTilesX; x++) {
				// Position the floor tile sprite
				floorSprite.setPosition(startX + x * 48, startY + y * 48);

				// Draw the floor tile sprite
				window.draw(floorSprite);
			}
		}
	}
};