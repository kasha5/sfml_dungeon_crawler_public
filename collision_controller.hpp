#pragma once

class CollisionController {
private:
	std::vector<sf::FloatRect*> roomBounds;
	std::vector<sf::FloatRect*> corridorBounds;

	sf::Vector2f tl;
	sf::Vector2f tr;
	sf::Vector2f bl;
	sf::Vector2f br;

	sf::FloatRect* nearestRoom;

	bool outsideBounds;

	sf::Vector2f characterPosition;
	sf::FloatRect characterBounds;

	void createCollisionPoints() 
	{
		tl = sf::Vector2f(characterBounds.left, characterBounds.top + characterBounds.height / 2);
		tr = sf::Vector2f(characterBounds.left + characterBounds.width, characterBounds.top + characterBounds.height / 2);
		bl = sf::Vector2f(characterBounds.left, characterBounds.top + characterBounds.height);
		br = sf::Vector2f(characterBounds.left + characterBounds.width, characterBounds.top + characterBounds.height);
	}

	bool checkPointCollision(const sf::Vector2f& point)
	{
		for (sf::FloatRect* roomBounds : roomBounds) {
			if (roomBounds->contains(point)) {
				return true;
			}
		}
		for (sf::FloatRect* corridorBounds : corridorBounds) {
			if (corridorBounds->contains(point)) {
				return true;
			}
		}
		return false;
	}

	bool pointOutsideBounds() 
	{
		if (!checkPointCollision(tl)) {
			findClosestRoom(tl);
			return true;
		}
		if (!checkPointCollision(tr)) {
			findClosestRoom(tr);
			return true;
		}
		if (!checkPointCollision(bl)) {
			findClosestRoom(bl);
			return true;
		}
		if (!checkPointCollision(br)) {
			findClosestRoom(br);
			return true;
		}

		return false;
	}

	void findClosestRoom(const sf::Vector2f& point) 
	{
		float nearestDistance = std::numeric_limits<float>::max();

		for (sf::FloatRect* room : roomBounds) {

			sf::Vector2f newPosition = getNewPosition(room);
			float distance = std::sqrt(std::pow(characterPosition.x - newPosition.x, 2) + std::pow(characterPosition.y - newPosition.y, 2));

			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				nearestRoom = room;
			}
		}

		for (sf::FloatRect* room : corridorBounds) {

			sf::Vector2f newPosition = getNewPosition(room);
			float distance = std::sqrt(std::pow(characterPosition.x - newPosition.x, 2) + std::pow(characterPosition.y - newPosition.y, 2));

			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				nearestRoom = room;
			}
		}
	}

	sf::Vector2f& getNewPosition(const sf::FloatRect* room) 
	{
		sf::Vector2f position;
		position.x = std::max(room->left + characterBounds.width / 2, std::min(characterPosition.x, room->left + room->width - characterBounds.width / 2));
		position.y = std::max(room->top + characterBounds.height / 2, std::min(characterPosition.y, room->top + room->height));
		return position;
	}

public:
	CollisionController() : outsideBounds(false), nearestRoom(nullptr) {}

	~CollisionController() {
		for (sf::FloatRect* room : roomBounds) delete room;
		for (sf::FloatRect* corridor : corridorBounds) delete corridor;
	}

	void load(const std::vector<Room*> rooms, const std::vector<Corridor*> corridors)
	{
		// Create bounds for rooms and corridors
		roomBounds.clear();
		for (const Room* room : rooms) {
			const sf::Vector2f position(room->x * tileSize.x, room->y * tileSize.y);
			const sf::Vector2f size(room->width * tileSize.x, room->height * tileSize.y);
			roomBounds.push_back(new sf::FloatRect(position, size));
		}

		corridorBounds.clear();
		for (const Corridor* corridor : corridors) {
			const sf::Vector2f position(corridor->x1 * tileSize.x, corridor->y1 * tileSize.y);
			const sf::Vector2f size(corridor->width * tileSize.x, corridor->height * tileSize.y);
			corridorBounds.push_back(new sf::FloatRect(position, size));
		}
	}

	void update(Character* character) 
	{
		characterPosition = character->getPosition();
		characterBounds = character->getGlobalBounds();

		createCollisionPoints();

		if (pointOutsideBounds()) {
			character->setPosition(getNewPosition(nearestRoom));
		}
	}
};