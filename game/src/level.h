#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"

typedef struct Entity Entity;

typedef struct Tile
{
	Vector3 bottomLeft;
	Vector3 bottomRight;
	Vector3 topLeft;
	Vector3 topRight;
	Vector2 tileCenterPos;

	Texture texture;
	float entityPos;

	// Gameplay variables

	Entity* entity;
	bool walkable;

} Tile;

typedef struct SpawnZone
{
	Tile* tiles[128];
	
	int playerID;
	int numTiles;

} SpawnZone;

#endif