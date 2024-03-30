#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"

typedef struct Tile Tile;

enum EntityType
{
	ENTITY_TYPE_CHARACTER,
	ENTITY_TYPE_TERRAIN_OBJECT
};

typedef struct Entity
{
	// Rendering variables

	Vector2 size;
	Vector3 position;
	
	Texture texture;
	Texture deathTexture;
	Rectangle textureRect;
	BoundingBox boundingBox;

	// Gameplay variables

	Tile* tile;

	bool isActive;
	bool isAlive;
	bool isBlockingMovement;
	int teamID;
	int type;
	int speed;					// How many tiles can the unit move.
	int baseInitiative;			// Initiative value reset after a finished turn.
	int currentInitiative;		// Determines position in turn queue.
	char name[256];

	// Attributes
	int strength; // -> lyönti (kipumäärä), hp
	int agility; // -> nopeus, initiative?
	int dexterity; // -> tarkkuus, kätevyys, ketteryys, kriittisyys
	int wisdom; // -> mana, tietäjyys, "magian endurance", magioiden määrä muistissa???
	int intelligence; // -> magian iskumäärä, älykkyys -> good for puzzles
	int endurance; // -> kantokyky, hp
	int perception; // -> kartoituskyky, traps etc., critical++
	int charisma; // -> hinnoittelu kaupankäynnissä, voi hurmata, sosiaalinen taito, voi puhua ongelmista
	int luck; // -> tuuri, munkki, säkä, vaikuttaa kaikkeen

	// DOTA BEST ja Warcraft 3
	//int strength;  
	//int agility;
	//int intelligence;

	// 1 - strength +1, agility +1
	// 2 - intelligence +1
	// 3 - strength +2, intelligence +1
	// 4 - ...

	int weapon;
	int armor;
	int item;

	int poisonResistance; // Jos max niin immunity
	int fireResistance;
	int coldResistance;
	int waterResistance;
	int physicalResistance;
	int mentalResistance;
	int elementalResistance;
	int magicResistance;
} Entity;

typedef struct Perk
{
	int id;
} Perk;

typedef struct Weapon
{
	int id;
	int type;
	int perkIds[3];

	int strength; // Jos piikkejä -> vittu et sattuu
	int agility; // Kevyt -> liikkuu nopeemmin, raskas -> ei liiku
	int intelligence; // Jos on kaapu -> huoh, lunttilappu hihassa

	int poisonResistance; // Jos max niin immunity
	int fireResistance;
	int coldResistance;
	int waterResistance;
	int physicalResistance;
	int mentalResistance;
	int elementalResistance;
	int magicResistance;
} Weapon;

typedef struct Armor
{
	int id;
	int type;
	int perkIds[3];

	int strength; // Jos piikkejä -> vittu et sattuu
	int agility; // Kevyt -> liikkuu nopeemmin, raskas -> ei liiku
	int intelligence; // Jos on kaapu -> huoh, lunttilappu hihassa

	int poisonResistance; // Jos max niin immunity
	int fireResistance;
	int coldResistance;
	int waterResistance;
	int physicalResistance;
	int mentalResistance;
	int elementalResistance;
	int magicResistance;
} Armor;

typedef struct Artifact
{
	int id;
	int type;
	int perkIds[3];

	int strength; // Jos piikkejä -> vittu et sattuu
	int agility; // Kevyt -> liikkuu nopeemmin, raskas -> ei liiku
	int intelligence; // Jos on kaapu -> huoh, lunttilappu hihassa

	int poisonResistance; // Jos max niin immunity
	int fireResistance;
	int coldResistance;
	int waterResistance;
	int physicalResistance;
	int mentalResistance;
	int elementalResistance;
	int magicResistance;
} Artifact;

#endif