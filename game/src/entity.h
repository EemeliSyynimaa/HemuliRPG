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

} Entity;

#endif