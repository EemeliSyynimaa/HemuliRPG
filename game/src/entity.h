#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"

typedef struct Entity
{
	Vector2 size;
	Vector3 position;
	
	Texture texture;
	Rectangle textureRect;
	BoundingBox boundingBox;
} Entity;

#endif