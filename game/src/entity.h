#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"

typedef struct Entity
{
	// Rendering variables

	Vector2 size;
	Vector3 position;
	
	Texture texture;
	Rectangle textureRect;
	BoundingBox boundingBox;

	// Gameplay variables

	int teamID;

} Entity;

#endif