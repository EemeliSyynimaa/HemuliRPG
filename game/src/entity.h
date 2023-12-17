#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"

typedef struct Tile Tile;

typedef struct Entity
{
	// Rendering variables

	Vector2 size;
	Vector3 position;
	
	Texture texture;
	Rectangle textureRect;
	BoundingBox boundingBox;

	// Gameplay variables

	Tile* tile;

	bool isActive;
	bool isAlive;
	int teamID;
	int speed;

} Entity;

#endif