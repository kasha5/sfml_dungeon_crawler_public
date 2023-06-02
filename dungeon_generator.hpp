#pragma once

class Room {
private:

	unsigned int x, y, width, height;

public:

	Room(int _x, int _y, int _width, int _height) : x(_x), y(_y), width(_width), height(_height) {}

	unsigned int getWidth() const { return width; }
	unsigned int getHeight() const { return height; }
	unsigned int getX() const { return x; }
	unsigned int getY() const { return y; }

	friend class BSPDungeon;
	friend class MapRenderer;
	friend class CollisionController;
};

class Corridor {
private:

	int x1, y1, x2, y2;
	int width, height;

	Corridor(int _x1, int _y1, int _x2, int _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2) 
	{
		float angle = std::atan2(y2 - y1, x2 - x1);
		float length = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

		if (angle > 0) {
			width = corridorWidth; 
			height = length;
		}
		else {
			width = length;
			height = corridorWidth;
		}
	}

	friend class BSPDungeon;
	friend class MapRenderer;
	friend class CollisionController;
};

class Node {
private:

	int x, y, width, height;

	Node* left;
	Node* right;
	Room* room;

	Node(int _x, int _y, int _width, int _height) : x(_x), y(_y), width(_width), height(_height), left(nullptr), right(nullptr), room(nullptr) {}

	friend class BSPDungeon;
};

class BSPDungeon {
private:

	Node* root;

	int width;
	int height;

	std::vector<Room*> rooms;
	std::vector<Corridor*> corridors;

	Room* spawnRoom = nullptr;
	Room* bossRoom = nullptr;
	std::vector<Room*> chestRooms;
	bool chestRoom = false;

	void splitNode(Node* node);

	void splitHorizontal(Node* node);

	void splitVertical(Node* node);

	void deleteNode(Node* node);

	void generateRooms(Node* node);

	void generateCorridors(Node* node);

	void pickSpawnRoom(Node* node);

	void pickBossRoom(Node* node, const bool& pick);

	void pickChestRoom(Node* node);

	void reset() 
	{
		deleteNode(root);
		for (auto corridor : corridors) delete corridor;
		rooms.clear();
		corridors.clear();
		chestRooms.clear();
		spawnRoom = nullptr; 
		bossRoom = nullptr;
		root = nullptr;
	}

public:

	BSPDungeon(int _width, int _height) : width(_width), height(_height), root(nullptr) {}

	~BSPDungeon() { reset(); }

	void generate();

	std::vector<Room*> getRooms() const { return rooms; }

	Room* getBossRoom() const { return bossRoom; }

	Room* getSpawnRoom() const { return spawnRoom; }

	std::vector<Corridor*> getCorridors() const { return corridors; }

	std::vector<Room*> getChestRooms() const { return chestRooms; }

	sf::Vector2f getStartingPosition() const { return sf::Vector2f((spawnRoom->x + spawnRoom->width / 2) * tileSize.x, (spawnRoom->y + spawnRoom->height / 2) * tileSize.y); }
};

void BSPDungeon::generate() 
{
	reset();

	root = new Node(20, 20, width, height);

	splitNode(root);
	generateRooms(root);
	generateCorridors(root);

	bool pick = getRandomInRange(0, 100) < 50;
	if (getRandomInRange(0, 100) < 50) {
		pickSpawnRoom(root->left);
		pickBossRoom(root->right, pick);
	}
	else {
		pickSpawnRoom(root->right);
		pickBossRoom(root->left, pick);
	}

	chestRoom = false;

	pickChestRoom(root->left);

	chestRoom = false;

	pickChestRoom(root->right);
}

void BSPDungeon::splitNode(Node* node) 
{
	if (node == nullptr) return;

	// Randomly pick split direction
	bool splitHorizontally = getRandomInRange(0, 100) < 50;

	if (splitHorizontally) {
		// Split if there is enough space for two subdungeons
		if (node->height > 2 * maxRoomSize) {
			splitHorizontal(node);
		}
		else if (node->width > 2 * minRoomSize) {
			splitVertical(node);
		}
		else return;
	}
	else {
		if (node->width > 2 * maxRoomSize) {
			splitVertical(node);
		}
		else if (node->height > 2 * minRoomSize) {
			splitHorizontal(node);
		}
		else return;
	}

	splitNode(node->left);
	splitNode(node->right);
}

void BSPDungeon::deleteNode(Node* node) 
{
	if (node == nullptr) return;
	deleteNode(node->left);
	deleteNode(node->right);
	delete node->room;
	node->room = nullptr;
	delete node;
}

void BSPDungeon::splitHorizontal(Node* node) 
{
	int maxSize = node->height - minRoomSize;
	int h1 = getRandomInRange(minRoomSize, maxSize);
	int h2 = node->height - h1;
	node->left = new Node(node->x, node->y, node->width, h1);
	node->right = new Node(node->x, node->y + node->left->height, node->width, h2);
}

void BSPDungeon::splitVertical(Node* node) 
{
	int maxSize = node->width - minRoomSize;
	int w1 = getRandomInRange(minRoomSize, maxSize);
	int w2 = node->width - w1;
	node->left = new Node(node->x, node->y, w1, node->height);
	node->right = new Node(node->x + node->left->width, node->y, w2, node->height);
}

void BSPDungeon::generateRooms(Node* node)
{
	if (node == nullptr) return;

	if (node->left == nullptr || node->right == nullptr) {
		// Choose random position for a room within corresponding subdungeon
		int x = node->x + getRandomInRange(roomMargin, node->width / 3);
		int y = node->y + getRandomInRange(roomMargin, node->height / 3);

		// Calculate room dimensions based on the position
		int w = node->width - (x - node->x);
		int h = node->height - (y - node->y);
		w -= getRandomInRange(roomMargin, w / 3);
		h -= getRandomInRange(roomMargin, h / 3);

		node->room = new Room(x, y, w, h);
		rooms.push_back(node->room);
		return;
	}

	generateRooms(node->left);
	generateRooms(node->right);
}

void BSPDungeon::generateCorridors(Node* node) 
{
	if (node->left == nullptr || node->right == nullptr) return;

	int start_x = node->left->x + (node->left->width / 2);
	int start_y = node->left->y + (node->left->height / 2);
	int end_x = node->right->x + (node->right->width / 2);
	int end_y = node->right->y + (node->right->height / 2);

	corridors.push_back(new Corridor(start_x, start_y, end_x, end_y));

	generateCorridors(node->left);
	generateCorridors(node->right);
}

void BSPDungeon::pickSpawnRoom(Node* node) 
{
	if (spawnRoom != nullptr) return;

	if (node->left == nullptr || node->right == nullptr) {
		spawnRoom = node->room;
		return;
	}

	bool pickLeft = getRandomInRange(0, 100) < 50;

	if (pickLeft) {
		pickSpawnRoom(node->left);
	}
	else {
		pickSpawnRoom(node->right);
	}
}

void BSPDungeon::pickBossRoom(Node* node, const bool& pick) 
{
	if (bossRoom != nullptr) return;

	if (node->left == nullptr || node->right == nullptr) {
		bossRoom = node->room;
		return;
	}

	if (pick) {
		pickBossRoom(node->left, pick);
	}
	else {
		pickBossRoom(node->right, pick);
	}
}

void BSPDungeon::pickChestRoom(Node* node) 
{
	if (chestRoom) return;

	if (node->left == nullptr || node->right == nullptr) 
	{
		if (node->room != bossRoom && node->room != spawnRoom) {
			chestRooms.push_back(node->room);
			chestRoom = true;
		}
		return;
	}

	pickChestRoom(node->left);
	pickChestRoom(node->right);
}