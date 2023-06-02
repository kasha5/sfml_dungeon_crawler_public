#pragma once

enum GameState { gameLoop, gameEndWin, gameEndLost, exitMenu };

class Game {
private:

	sf::RenderWindow window;
	sf::View view;

	BSPDungeon* currentDungeon;
	MapRenderer* mapRenderer;
	BackgroundRenderer* backgroundRenderer;

	PlayerCharacter* playerCharacter;

	CollisionController* collisionController;

	EnemyController* enemyController;

	WeaponContainer* weaponPool;
	WeaponContainer* weaponsOnGround;
	ChestContainer* chestContainer;
	ItemContainer* potionContainer;

	EndGameScreen* endGameScreen;
	PotionStatus* potionStatus;

	sf::Clock frameClock;

	bool generateLevel;
	unsigned int currentLevel;

	GameState gameState;

public:

	Game(unsigned int window_width, unsigned int window_height) :
		window(sf::VideoMode(window_width, window_height), "SFML Window", sf::Style::Fullscreen), view(sf::Vector2f(0.f, 0.f), sf::Vector2f(cameraSizeX, cameraSizeY)),
		currentDungeon(nullptr), mapRenderer(nullptr), backgroundRenderer(nullptr), playerCharacter(nullptr), collisionController(nullptr), enemyController(nullptr), weaponPool(nullptr), endGameScreen(nullptr)
	{
		window.setFramerateLimit(defaultFPS);
		window.setView(view);

		endGameScreen = new EndGameScreen(window);

		potionStatus = new PotionStatus(window);

		restartGame();
	}

	~Game()
	{
		delete currentDungeon;
		delete mapRenderer;
		delete backgroundRenderer;
		delete playerCharacter;
		delete collisionController;
		delete enemyController;
		delete endGameScreen;
		delete weaponPool;
		delete weaponsOnGround;
		delete chestContainer;
		delete potionContainer;
		delete potionStatus;
	}

	void generateDungeon(unsigned int width, unsigned int height, std::string enemyTexturePath, unsigned int bossHP, float bossMvSpeed)
	{
		delete currentDungeon;
		currentDungeon = new BSPDungeon(width, height);
		currentDungeon->generate();

		delete collisionController;
		collisionController = new CollisionController;
		collisionController->load(currentDungeon->getRooms(), currentDungeon->getCorridors());

		delete enemyController;
		enemyController = new EnemyController;
		enemyController->loadEnemies(enemyTexturePath);
		enemyController->spawnEnemies(currentDungeon->getRooms(), currentDungeon->getBossRoom(), currentDungeon->getSpawnRoom(), bossHP, bossMvSpeed);

		weaponsOnGround->passWeaponsToAnotherContainer(weaponPool);

		chestContainer->spawnChests(currentDungeon->getChestRooms());

		potionContainer->reset();

		playerCharacter->setPosition(currentDungeon->getStartingPosition());
	}

	void createMap(std::string dungeonTileset, std::string background1Tileset, sf::Vector2u tileSize, unsigned int backgroundWidth, unsigned int backgroundHeight)
	{
		delete backgroundRenderer;
		backgroundRenderer = new BackgroundRenderer;
		backgroundRenderer->load(background1Tileset, tileSize, backgroundWidth, backgroundHeight);

		delete mapRenderer;
		mapRenderer = new MapRenderer;
		mapRenderer->load(dungeonTileset, tileSize, currentDungeon->getRooms(), currentDungeon->getCorridors());
	}

	void createPlayer(std::string idleAnimPath, std::string runAnimPath, unsigned int mv_speed, unsigned int HP)
	{
		delete playerCharacter;
		playerCharacter = new PlayerCharacter(idleAnimPath, runAnimPath, mv_speed, HP);
		playerCharacter->equipWeapon(weaponPool->getRandomWeapon());
	}

	void drawSprites()
	{
		window.draw(*backgroundRenderer);
		window.draw(*mapRenderer);

		std::vector<sf::Sprite> chestSprites = chestContainer->getChestSprites();
		for (sf::Sprite& sprite : chestSprites) {
			window.draw(sprite);
		}
		
		std::vector<sf::Sprite> weaponSprites = weaponsOnGround->getWeaponSprites();
		for (sf::Sprite& sprite : weaponSprites) {
			window.draw(sprite);
		}

		std::vector<sf::Sprite> potionSprites = potionContainer->getSprites();
		for (sf::Sprite& sprite : potionSprites) {
			window.draw(sprite);
		}

		std::vector<sf::Sprite> enemySprites = enemyController->getEnemySprites();
		for (sf::Sprite& sprite : enemySprites) {
			window.draw(sprite);
		}

		std::vector<sf::RectangleShape> enemyHealthbars = enemyController->getEnemyHealthbars();
		for (sf::RectangleShape& rect : enemyHealthbars) {
			window.draw(rect);
		}

		window.draw(playerCharacter->getSprite());
		window.draw(playerCharacter->getWeaponSprite());
		window.draw(playerCharacter->getHealthbar());
		
		potionStatus->render(view, playerCharacter);
	}

	void updateModules(const float& dt)
	{
		sf::Vector2f healthBarPosition = view.getCenter() - view.getSize() / 2.f;
		healthBarPosition.x += 10.f;
		healthBarPosition.y += 10.f;
		playerCharacter->update(dt, healthBarPosition);
		collisionController->update(playerCharacter);

		chestContainer->update(dt, playerCharacter);
		weaponsOnGround->update(playerCharacter);

		potionContainer->update(playerCharacter);

		enemyController->update(dt, playerCharacter, collisionController, potionContainer);
	}

	void gameStateUpdater() 
	{
		if (gameState == exitMenu) return;

		if (playerCharacter->getCurrentHP() <= 0) {
			gameState = gameEndLost;
			return;
		}

		if (enemyController != nullptr && enemyController->bossDefeated()) generateLevel = true;

		if (generateLevel || currentDungeon == nullptr) {
			if (currentLevel == 1) {
				generateDungeon(dungeon1width, dungeon1height, dungeon1EnemiesDir, boss1HP, boss1MvSpeed);
				createMap(dungeon1Tileset, background1Tileset, tileSize, dungeon1width + 40, dungeon1height + 40);
			}
			else if (currentLevel == 2) {
				generateDungeon(dungeon2width, dungeon2height, dungeon2EnemiesDir, boss2HP, boss2MvSpeed);
				createMap(dungeon2Tileset, background2Tileset, tileSize, dungeon2width + 40, dungeon2height + 40);
			}
			else if (currentLevel == 3) {
				generateDungeon(dungeon3width, dungeon3height, dungeon3EnemiesDir, boss3HP, boss3MvSpeed);
				createMap(dungeon3Tileset, background3Tileset, tileSize, dungeon3width + 40, dungeon3height + 40);
			}
			else {
				gameState = gameEndWin;
				return;
			}
			currentLevel++;
			generateLevel = false;
		}
	}

	void restartGame()
	{
		generateLevel = true;
		currentLevel = 1;
		gameState = gameLoop;

		delete weaponPool;
		weaponPool = new WeaponContainer;
		weaponPool->load(weaponsDir);

		delete weaponsOnGround;
		weaponsOnGround = new WeaponContainer;

		delete chestContainer;
		chestContainer = new ChestContainer(weaponPool, weaponsOnGround);

		delete potionContainer;
		potionContainer = new ItemContainer;

		createPlayer(knightIdleAnim, knightRunAnim, 8, 6);
	}

	void renderGame(const float& dt)
	{
		if (gameState == gameLoop)
		{
			view.setCenter(playerCharacter->getPosition());
			window.setView(view);

			updateModules(dt);
			drawSprites();
		}
		else if (gameState == gameEndLost)
		{
			endGameScreen->setText("You Lost!\nPress [ENTER] to restart the game\nor [ESC] to exit the game");
			endGameScreen->render(view);
			if (endGameScreen->handleInput())
			{
				restartGame();
			}
		}
		else if (gameState == gameEndWin) 
		{
			endGameScreen->setText("Congrats, You won!\nPress [ENTER] to restart the game\nor [ESC] to exit the game");
			endGameScreen->render(view);
			if (endGameScreen->handleInput())
			{
				restartGame();
			}
		}
		else if (gameState == exitMenu)
		{
			endGameScreen->setText("Exit the game?\nPress [ENTER] to confirm\nor [LMB] to keep playing");
			endGameScreen->render(view);

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) gameState = gameLoop;
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) window.close();
		}
	}

	void startGame()
	{
		while (window.isOpen())
		{
			float dt = frameClock.restart().asSeconds();

			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
				}

				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
					if (gameState == gameLoop) gameState = exitMenu;
				}
			}

			gameStateUpdater();

			view.setCenter(playerCharacter->getPosition());
			window.setView(view);

			window.clear();

			renderGame(dt);

			window.display();
		}
	}
};