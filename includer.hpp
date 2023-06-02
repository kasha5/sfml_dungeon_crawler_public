#pragma once

// Libraries

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// Game parameters

static const unsigned int defaultFPS = 144;

static const float cameraSizeX = 500.f;
static const float cameraSizeY = 300.f;

static const sf::Vector2u tileSize(16, 16);

// Dungeon parameters

static const unsigned int minRoomSize = 14;
static const unsigned int maxRoomSize = 16;
static const unsigned int roomMargin = 2;
static const unsigned int corridorWidth = 2;

static const unsigned int dungeon1width = 40;
static const unsigned int dungeon1height = 60;

static const unsigned int dungeon2width = 60;
static const unsigned int dungeon2height = 80;

static const unsigned int dungeon3width = 80;
static const unsigned int dungeon3height = 100;

// Potions

static const std::string healPotionTexture = "./assets/potions/heal_potion.png";
static const unsigned int healPotionChance = 5;

static const std::string speedPotionTexture = "./assets/potions/speed_potion.png";
static const unsigned int speedPotionChance = 10;

static const std::string invincibilityPotionTexture = "./assets/potions/invincibility_potion.png";
static const unsigned int invincibilityPotionChance = 15;

// Enemy Parameters

static const unsigned int tilesPerOneCapacityPoint = 24;

static const unsigned int tier1EnemyChance = 30;
static const unsigned int tier2EnemyChance = 30;
static const unsigned int tier3EnemyChance = 20;

static const unsigned int tier1EnemyMvSpeed = 7;
static const unsigned int tier2EnemyMvSpeed = 6;
static const unsigned int tier3EnemyMvSpeed = 5;

static const unsigned int tier1EnemyHP = 15;
static const unsigned int tier2EnemyHP = 25;
static const unsigned int tier3EnemyHP = 35;

static const unsigned int boss1HP = 100;
static const float boss1MvSpeed = 6.f;

static const unsigned int boss2HP = 150;
static const float boss2MvSpeed = 6.5f;

static const unsigned int boss3HP = 200;
static const float boss3MvSpeed = 7.f;

// Weapon parameters

static const unsigned int fastWeaponDamage = 5;
static const float fastWeaponAttackCooldown = 0.2;

static const unsigned int mediumWeaponDamage = 10;
static const float mediumWeaponAttackCooldown = 0.3;

static const unsigned int slowWeaponDamage = 15;
static const float slowWeaponAttackCooldown = 0.4;

// Asset file locations

static const std::string dungeon1Tileset = "./assets/dungeon1/map/tileset.png";
static const std::string background1Tileset = "./assets/dungeon1/background/background.png";
static const std::string dungeon1EnemiesDir = "./assets/dungeon1/enemies/";

static const std::string dungeon2Tileset = "./assets/dungeon2/map/tileset.png";
static const std::string background2Tileset = "./assets/dungeon2/background/background.png";
static const std::string dungeon2EnemiesDir = "./assets/dungeon2/enemies/";

static const std::string dungeon3Tileset = "./assets/dungeon3/map/tileset.png";
static const std::string background3Tileset = "./assets/dungeon3/background/background.png";
static const std::string dungeon3EnemiesDir = "./assets/dungeon3/enemies/";

static const std::string healthbarTexture = "./assets/healthbar/healthbar.png";

static const std::string weaponsDir = "./assets/weapons/";

static const std::string chestOpenAnim = "./assets/chest/chest_empty_open_anim_f";

static const std::string elfIdleAnim = "./assets/heroes/elf/elf_m_idle_anim_f";
static const std::string elfRunAnim = "./assets/heroes/elf/elf_m_run_anim_f";

static const std::string knightIdleAnim = "./assets/heroes/knight/knight_m_idle_anim_f";
static const std::string knightRunAnim = "./assets/heroes/knight/knight_m_run_anim_f";

static const std::string lizardIdleAnim = "./assets/heroes/lizard/lizard_m_idle_anim_f";
static const std::string lizardRunAnim = "./assets/heroes/lizard/lizard_m_run_anim_f";

static const std::string wizardIdleAnim = "./assets/heroes/wizzard/wizzard_m_idle_anim_f";
static const std::string wizardRunAnim = "./assets/heroes/wizzard/wizzard_m_run_anim_f";

// Functions

int getRandomInRange(int min, int max) 
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);

	std::uniform_int_distribution<int> distribution(min, max);

	return distribution(generator);
}

// Header files

#include "dungeon_generator.hpp"
#include "map_renderer.hpp"
#include "animation.hpp"
#include "weapon.hpp"
#include "items.hpp"
#include "character.hpp"
#include "chest.hpp"
#include "collision_controller.hpp"
#include "enemy_controller.hpp"
#include "interface_elements.hpp"
#include "utilities.hpp"