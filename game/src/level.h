#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"

typedef struct Tile
{
	Vector3 bottomLeft;
	Vector3 bottomRight;
	Vector3 topLeft;
	Vector3 topRight;

	Texture texture;

	float entityPos;

	// Gameplay stuff here?

} Tile;

#endif