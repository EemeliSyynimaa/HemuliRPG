/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include "rlgl.h"
#include "raymath.h"
#include "rcamera.h"

#include "entity.h"
#include "level.h"
#include "button.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
#define MAP_WIDTH 10
#define MAP_HEIGHT 8

#define MAP_HEIGHT_VERTICES MAP_HEIGHT + 1
#define MAP_WIDTH_VERTICES MAP_WIDTH + 1
#define TILE_SIZE 1

#define MAX_ENTITIES 24
#define SPAWN_ZONES 2        // MAP DATA KNOWS HOW MANY ZONES.

void DrawQuad3D(Camera camera, Vector3 bottomLeft, Vector3 bottomRight, Vector3 topRight, Vector3 topLeft, Color tint)
{
    rlBegin(RL_QUADS);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);

        // Bottom-left corner for texture and quad
        rlTexCoord2f(0.0f, 0.0f);
        rlVertex3f(topLeft.x, topLeft.y, topLeft.z);

        // Top-left corner for texture and quad
        rlTexCoord2f(0.0f, 1.0f);
        rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);

        // Top-right corner for texture and quad
        rlTexCoord2f(1.0f, 1.0f);
        rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);

        // Bottom-right corner for texture and quad
        rlTexCoord2f(1.0f, 0.0f);
        rlVertex3f(topRight.x, topRight.y, topRight.z);

    rlEnd();
}

void DrawRectangle3D(Camera camera, Vector3 position, Vector2 size, Color tint)
{
    Vector3 topLeft = Vector3Add(position, (Vector3) { 0, 0, size.y });
    Vector3 topRight = Vector3Add(position, (Vector3) { size.x, 0, size.y });
    Vector3 bottomRight = Vector3Add(position, (Vector3) { size.x, 0, 0 });
    Vector3 bottomLeft = position;

    DrawQuad3D(camera, bottomLeft, bottomRight, topRight, topLeft, tint);
}

void DrawGameGrid(int mapWidth, int mapHeight, int spacing)
{
    float depth = -0.001f;

    rlBegin(RL_LINES);

    for (int i = 0; i <= mapWidth; i++)
    {
        rlColor3f(0.75f, 0.75f, 0.75f);
        rlColor3f(0.75f, 0.75f, 0.75f);
        rlColor3f(0.75f, 0.75f, 0.75f);
        rlColor3f(0.75f, 0.75f, 0.75f);

        rlVertex3f((float)i * spacing, depth, (float)0.0f * spacing);
        rlVertex3f((float)i * spacing, depth, (float)mapHeight * spacing);

        // Original code.
        //rlVertex3f((float)0.0f * spacing, 0.0f, (float)i * spacing);
        //rlVertex3f((float)MAP_HEIGHT * spacing, 0.0f, (float)i * spacing);
    }
    for (int i = 0; i <= mapHeight; i++)
    {
        rlColor3f(0.75f, 0.75f, 0.75f);
        rlColor3f(0.75f, 0.75f, 0.75f);
        rlColor3f(0.75f, 0.75f, 0.75f);
        rlColor3f(0.75f, 0.75f, 0.75f);

        // Original code.
        //rlVertex3f((float)i * spacing, 0.0f, (float)0.0f * spacing);
        //rlVertex3f((float)i * spacing, 0.0f, (float)MAP_WIDTH * spacing);

        rlVertex3f((float)0.0f * spacing, depth, (float)i * spacing);
        rlVertex3f((float)mapWidth * spacing, depth, (float)i * spacing);
    }

    rlEnd();
}

void DrawEntities(Entity entities[], int numEntities, int selectedUnitID, Camera camera)
{
    // HAX: pls fix at some point, MAX_ENTITIES?
    int renderQueue[128] = { 0 };

    for (int i = 0; i < numEntities; i++)
    {
        renderQueue[i] = i;
    }

    for (int i = 0; i < numEntities - 1; i++)
    {
        float distanceA = Vector3Distance(entities[renderQueue[i]].position, camera.position);

        for (int j = i + 1; j < numEntities; j++)
        {
            float distanceB = Vector3Distance(entities[renderQueue[j]].position, camera.position);

            if (distanceA < distanceB)
            {
                int temp = renderQueue[i];
                renderQueue[i] = renderQueue[j];
                renderQueue[j] = temp;
                distanceA = distanceB;
            }
        }
    }

    for (int i = 0; i < numEntities; i++)
    {
        Entity* entity = &entities[renderQueue[i]];
        Vector3 entityPos = entity->position;

        // Don't render deactivated units
        if (entity->isActive == false)
        {
            continue;
        }

        Vector3 up = { 0.0f, -1.0f, 0.0f };
        Vector2 origin = Vector2Zero();
        float rotation = 0.0f;
        Color tint = WHITE;

        // Render separate from map position.
        entityPos.x += 0.5f;
        entityPos.y += -0.5f;
        entityPos.z += 0.5f;

        Texture texture = entity->texture;
        if (entity->isAlive == false)
        {
            texture = entity->deathTexture;
        }

        // Draw unit/entity.
        DrawBillboardPro(camera, texture, entity->textureRect, entityPos, up, entity->size, origin, rotation, tint);

        if (entity->maxHealth != 0)
        {
            // Draw healthbar.
            Texture healthTexture = blankTexture;
            Rectangle healthTextureRect = (Rectangle){ 0.0f, 0.0f, (float)healthTexture.width, (float)healthTexture.height };
            Vector3 healthPos = { 0.0f };
            healthPos.x = entityPos.x;
            healthPos.y = entityPos.y - entity->size.y * 0.5f - 0.1f;
            healthPos.z = entityPos.z;

            Vector3 backgroundPos = healthPos;

            Vector3 cameraVector = Vector3Normalize(Vector3Subtract(camera.position, camera.target));
            Vector3 cameraRightVector = Vector3Normalize(Vector3CrossProduct(camera.up, cameraVector));

            float healthPercentage = (float)entity->health / (float)entity->maxHealth;

            // Move healthbar color to the left.
            healthPos.x -= cameraRightVector.x * (1 - healthPercentage) * 0.5f;
            healthPos.z -= cameraRightVector.z * (1 - healthPercentage) * 0.5f;

            // Move background color to the right.
            backgroundPos.x += cameraRightVector.x * (healthPercentage) * 0.5f;
            backgroundPos.z += cameraRightVector.z * (healthPercentage) * 0.5f;

            Color healthBarColor = RED;
            if (entity->teamID == entities[selectedUnitID].teamID)
            {
                healthBarColor = BLUE;
            }

            DrawBillboardPro(camera, healthTexture, healthTextureRect, backgroundPos, up, (Vector2) { 1.0f - (1.0f * healthPercentage), 0.1f }, origin, rotation, DARKGRAY);

            // Draw health bar.
            DrawBillboardPro(camera, healthTexture, healthTextureRect, healthPos, up, (Vector2) { 1.0f * healthPercentage, 0.1f }, origin, rotation, healthBarColor);
        }
        // TODO FIX.
        /*if (entities[renderQueue[i]].teamID == currentTurnTeamID && entities[renderQueue[i]].type == ENTITY_TYPE_CHARACTER)
        {
            DrawBoundingBox(entities[renderQueue[i]].boundingBox, WHITE);
        }*/
    }
}

void DrawTiles(Tile* tileMap, int mapHeight, int mapWidth, Camera camera)
{
    for (int z = 0; z < mapHeight; z++)
    {
        for (int x = 0; x < mapWidth; x++)
        {
            Tile* tile = &tileMap[z * mapWidth + x];

            rlSetTexture(tile->texture.id);
            DrawQuad3D(camera, tile->bottomLeft, tile->bottomRight, tile->topRight, tile->topLeft, (z * mapHeight + x) % 2 ? WHITE : BLUE);

            rlSetTexture(0);
        }
    }
}

void DrawSelectionArea(Tile* selectionTileMap[], int numSelectionTiles, Entity* selectedEntity)
{
    Color colorNormal = { YELLOW.r, YELLOW.g, YELLOW.b, 96 };
    Color colorEnemy = { RED.r, RED.g, RED.b, 96 };
    Color colorAlly = { BLUE.r, BLUE.g, BLUE.b, 96 };
    Color colorSelected = { WHITE.r, WHITE.g, WHITE.b, 164 };

    // FIX THIS
    rlSetTexture(blankTexture.id);

    for (int i = 0; i < numSelectionTiles; i++)
    {
        Tile* tile = selectionTileMap[i];

        float offset = 0.02f;

        Color color = colorNormal;

        if (tile->entity)
        {
            if (tile->entity->type == ENTITY_TYPE_CHARACTER)
            {
                if (tile->entity == selectedEntity)
                {
                    color = colorSelected;
                }
                else if (tile->entity->teamID == selectedEntity->teamID)
                {
                    color = colorAlly;
                }
                else
                {
                    color = colorEnemy;
                }
            }
            else if (tile->entity->type == ENTITY_TYPE_TERRAIN_OBJECT)
            {
                continue;
            }
        }

        DrawQuad3D(camera,
            Vector3Add(tile->bottomLeft, (Vector3) { offset, -0.01f, offset }),
            Vector3Add(tile->bottomRight, (Vector3) { -offset, -0.01f, offset }),
            Vector3Add(tile->topRight, (Vector3) { -offset, -0.01f, -offset }),
            Vector3Add(tile->topLeft, (Vector3) { offset, -0.01f, -offset }),
            color);
    }
}

void UpdateGameCamera(Camera* camera)
{
    Vector2 mousePositionDelta = GetMouseDelta();

    bool moveInWorldPlane = true;
    bool rotateAroundTarget = true;
    bool lockView = true;
    bool rotateUp = false;

    float cameraMoveSpeed = 0.09f;
    float cameraRotationSpeed = 0.03f;
    float cameraMouseMoveSensitivity = 0.003f;

    // Camera rotation
    if (IsKeyDown(KEY_DOWN)) CameraPitch(camera, -cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
    if (IsKeyDown(KEY_UP)) CameraPitch(camera, cameraRotationSpeed, lockView, rotateAroundTarget, rotateUp);
    if (IsKeyDown(KEY_RIGHT)) CameraYaw(camera, -cameraRotationSpeed, rotateAroundTarget);
    if (IsKeyDown(KEY_LEFT)) CameraYaw(camera, cameraRotationSpeed, rotateAroundTarget);
    //if (IsKeyDown(KEY_Q)) CameraRoll(camera, -cameraRotationSpeed);
    //if (IsKeyDown(KEY_E)) CameraRoll(camera, cameraRotationSpeed);

    // Mouse support when right clicked.
    if (IsMouseButtonDown(1))
    {
        CameraYaw(camera, -mousePositionDelta.x * cameraMouseMoveSensitivity, rotateAroundTarget);
        CameraPitch(camera, -mousePositionDelta.y * cameraMouseMoveSensitivity, lockView, rotateAroundTarget, rotateUp);
    }

    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        cameraMoveSpeed = 0.24f;
    }

    // Keyboard support
    if (IsKeyDown(KEY_W)) CameraMoveForward(camera, cameraMoveSpeed, moveInWorldPlane);
    if (IsKeyDown(KEY_A)) CameraMoveRight(camera, -cameraMoveSpeed, moveInWorldPlane);
    if (IsKeyDown(KEY_S)) CameraMoveForward(camera, -cameraMoveSpeed, moveInWorldPlane);
    if (IsKeyDown(KEY_D)) CameraMoveRight(camera, cameraMoveSpeed, moveInWorldPlane);

    // Zoom target distance
    CameraMoveToTarget(camera, -GetMouseWheelMove());
    if (IsKeyPressed(KEY_KP_SUBTRACT)) CameraMoveToTarget(camera, 2.0f);
    if (IsKeyPressed(KEY_KP_ADD)) CameraMoveToTarget(camera, -2.0f);
}

//----------------------------------------------------------------------------------
// Gameplay Screen Variables Definition
//----------------------------------------------------------------------------------

static int framesCounter = 0;
static int finishScreen = 0;

static Tile tileMap[MAP_HEIGHT][MAP_WIDTH] = { 0 };
static float depthMap[MAP_HEIGHT_VERTICES][MAP_WIDTH_VERTICES];
static SpawnZone spawnZones[SPAWN_ZONES];

Entity entities[MAX_ENTITIES] = { 0 };
Entity* entityTurnQueue[MAX_ENTITIES] = { 0 };
int numEntities = 0;
int numEntityTurns = 0;

RayCollision hitMapWorld = { 0 };
Vector3 selectionRectPos = { 0 };
Tile* selectionTiles[MAP_HEIGHT * MAP_WIDTH] = { 0 };
int numSelectionTiles = 0;

int selection = -1;
bool targetingMode = false;

Button endTurnButton = { 0 };
Button attackButton = { 0 };

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
void SpawnCharacter(SpawnZone* spawnZone, Texture2D* texture, Texture2D* deathTexture, int speed, int baseInitiative, int health, int maxHealth, int minAttack, int maxAttack, char* name)
{
    int numTiles = spawnZone->numTiles;

    for (int i = 0; i < numTiles; i++)
    {
        int randomValue = GetRandomValue(0, numTiles - 1);
        if (spawnZone->tiles[randomValue]->entity == NULL)
        {
            Vector3 spawnPosition = { 0 };
            Tile* spawnTile = spawnZone->tiles[randomValue];

            spawnPosition.x = spawnTile->bottomLeft.x;
            spawnPosition.z = spawnTile->bottomLeft.z;
            spawnPosition.y = spawnTile->entityPos;

            Entity* entity = &entities[numEntities];
            entityTurnQueue[numEntityTurns] = entity;

            entity->isActive = true;
            entity->isAlive = true;
            entity->target = NULL;

            entity->position = spawnPosition;
            entity->size = (Vector2){ 1.0f, 1.0f };
            entity->texture = *texture;
            entity->deathTexture = *deathTexture;
            entity->textureRect = (Rectangle){ 0.0f, 0.0f, (float)(*texture).width, (float)(*texture).height };
            entity->teamID = spawnZone->playerID;
            entity->type = ENTITY_TYPE_CHARACTER;
            entity->isBlockingMovement = true;
            entity->speed = speed;
            TextCopy(entity->name, name);
            entity->baseInitiative = baseInitiative;
            entity->currentInitiative = baseInitiative;
            entity->health = health;
            entity->maxHealth = maxHealth;
            entity->minAttack = minAttack;
            entity->maxAttack = maxAttack;
            numEntities++;
            numEntityTurns++;

            entity->tile = spawnTile;
            spawnTile->entity = entity;

            return;
        }
    }
}

void SpawnTerrainObject(int x, int z, Texture2D* texture)
{
    Entity* entity = &entities[numEntities];
    numEntities++;

    entity->isActive = true;
    entity->isAlive = true;

    Tile* spawnTile = &tileMap[z][x];
    Vector3 spawnPosition = { 0 };

    spawnPosition.x = spawnTile->bottomLeft.x;
    spawnPosition.z = spawnTile->bottomLeft.z;
    spawnPosition.y = spawnTile->entityPos;

    entity->position = spawnPosition;
    entity->size = (Vector2){ 1.0f, 1.0f };
    entity->texture = *texture;
    entity->textureRect = (Rectangle){ 0.0f, 0.0f, (float)(*texture).width, (float)(*texture).height };
    entity->type = ENTITY_TYPE_TERRAIN_OBJECT;
    entity->isBlockingMovement = true;

    entity->tile = spawnTile;
    spawnTile->entity = entity;
}

void RemoveEntity(Entity* entity)
{
    entity->tile->entity = NULL;
    entity->isActive = false;
}

void KillEntity(Entity* entity)
{
    entity->isAlive = false;
    entity->isBlockingMovement = false;
}

bool IsEnemy(Entity* entity)
{
    Entity* selectedEntity = &entities[selection];

    if (selectedEntity->teamID != entity->teamID)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void SelectEntity(int entityIndex)
{
    numSelectionTiles = 0;
    selection = entityIndex;
    Entity* entity = &entities[selection];

    if (entity->isAlive == true)
    {
        for (int z = 0; z < MAP_HEIGHT; z++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                Tile* tile = &tileMap[z][x];
                float tileDistance = Vector2Distance((Vector2) { entity->position.x + 0.5f, entity->position.z + 0.5f }, tile->tileCenterPos);

                {   
                    // Add moveable tiles and tiles with an enemy entity in melee range.
                    if ((tileDistance <= entity->speed && tile->walkable) || (tileDistance <= (float)entity->speed + 1.45f && tile->entity && IsEnemy(tile->entity)))
                    {
                        selectionTiles[numSelectionTiles] = tile;
                        numSelectionTiles++;
                    }
                }
            }
        }
    }
}

bool IsTileSelectable(Tile* tile)
{
    for (int i = 0; i < numSelectionTiles; i++)
    {
        if (tile == selectionTiles[i])
        {
            return true;
        }
    }
    return false;
}

void SortEntityTurnQueue()
{
    for (int i = 0; i < numEntityTurns - 1; i++)
    {
        for (int j = i + 1; j < numEntityTurns; j++)
        {
            Entity* entityA = entityTurnQueue[i];
            Entity* entityB = entityTurnQueue[j];

            if (entityA->currentInitiative > entityB->currentInitiative)
            {
                entityTurnQueue[i] = entityB;
                entityTurnQueue[j] = entityA;
            }
        }
    }
}

void BeginTurn()
{
    SortEntityTurnQueue();
    Entity* currentEntity = NULL;

    for (int i = 0; i < numEntityTurns; i++)
    {
        if (entityTurnQueue[i]->isAlive)
        {
            currentEntity = entityTurnQueue[i];
            break;
        }
    }

    if (currentEntity == NULL)
    {
        finishScreen = 1;
        return;
    }
    else
    {
        SelectEntity((int)(currentEntity - &entities[0]));
        int selectionInitiative = currentEntity->currentInitiative;

        for (int i = 0; i < MAX_ENTITIES; i++)
        {
            Entity* entity = &entities[i];

            if (entity->isAlive && entity->type == ENTITY_TYPE_CHARACTER)
            {
                entity->currentInitiative -= selectionInitiative;
            }
        }
    }
}

void EndTurn()
{
    entities[selection].currentInitiative = entities[selection].baseInitiative;
    selection = -1;
    numSelectionTiles = 0;

    targetingMode = false;
    TextCopy(attackButton.text, "ATTACK");

    int teamUnitCount[] = { 0, 0 };
    
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        Entity* entity = &entities[i];
        
        if (entity->isAlive && entity->type == ENTITY_TYPE_CHARACTER)
        {
            teamUnitCount[entity->teamID]++;
        }
    }

    if (teamUnitCount[0] == 0 || teamUnitCount[1] == 0)
    {
        finishScreen = 1;
    }

    BeginTurn();
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    numEntities = 0;
    numEntityTurns = 0;

    // Initialize Level
    for (int z = 0; z < MAP_HEIGHT_VERTICES; z++)
    {
        for (int x = 0; x < MAP_WIDTH_VERTICES; x++)
        {
            depthMap[z][x] = GetRandomValue(-1, 1) / 5.0f;
        }
    }

    for (int z = 0; z < MAP_HEIGHT; z++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            Tile* tile = &tileMap[z][x];

            tile->bottomLeft = (Vector3){ (float)x, depthMap[z][x], (float)z };
            tile->bottomRight = (Vector3){ (float)x + TILE_SIZE, depthMap[z][x + 1], (float)z };
            tile->topRight = (Vector3){ (float)x + TILE_SIZE, depthMap[z + 1][x + 1], (float)z + TILE_SIZE };
            tile->topLeft = (Vector3){ (float)x, depthMap[z + 1][x], (float)z + TILE_SIZE };

            tile->tileCenterPos.x = (tile->bottomLeft.x + tile->topRight.x) / 2;
            tile->tileCenterPos.y = (tile->bottomLeft.z + tile->topRight.z) / 2;

            tile->entityPos = (depthMap[z][x] + depthMap[z][x + 1] + depthMap[z + 1][x + 1] + depthMap[z + 1][x]) / 4;
            tile->texture = grassTexture;
            tile->entity = NULL;
            tile->walkable = true;      // TODO: BASED ON BIOME
        }
    }

    // TODO: FIX TEAM ID / SPAWN ID STUFF
    // TODO: SELECT SPAWN TILE RANDOMLY INSTEAD OF ALL
    for (int i = 0; i < SPAWN_ZONES; i++)
    {
        spawnZones[i].playerID = i;
        spawnZones[i].numTiles = 8;

        for (int j = 0; j < spawnZones[i].numTiles; j++)
        {
            spawnZones[i].tiles[j] = &tileMap[j][i * (MAP_WIDTH - 1)];
        }
    }

    // Initialize and spawn Entities

    SpawnTerrainObject(4, 3, &treeTexture);
    SpawnTerrainObject(1, 5, &treeTexture);
    SpawnTerrainObject(2, 2, &rockTexture);

    SpawnCharacter(&spawnZones[0], &wizardTexture, &deadWizardTexture, 4, 6, 80, 80, 6, 12, "Pasi");
    SpawnCharacter(&spawnZones[0], &wizardTexture, &deadWizardTexture, 4, 6, 85, 85, 8, 14, "Kielo");
    SpawnCharacter(&spawnZones[0], &wizardTexture, &deadWizardTexture, 6, 4, 75, 75, 12, 22, "Gandalf");
         
    SpawnCharacter(&spawnZones[1], &orcTexture, &deadOrcTexture, 2, 10, 80, 120, 5, 15, "Siqu");
    SpawnCharacter(&spawnZones[1], &orcTexture, &deadOrcTexture, 3, 10, 130, 130, 16, 20, "Bab");
    SpawnCharacter(&spawnZones[1], &orcTexture, &deadOrcTexture, 3, 2, 100, 100, 14, 18, "Sukellushitsaaja");

    TextCopy(endTurnButton.text, "END TURN");
    endTurnButton.textColor = WHITE;
    endTurnButton.buttonColor = DARKGREEN;
    endTurnButton.hoverColor = GREEN;
    endTurnButton.rect.width = 200;
    endTurnButton.rect.height = 120;
    endTurnButton.rect.x = GetScreenWidth() - endTurnButton.rect.width;
    endTurnButton.rect.y = GetScreenHeight() - endTurnButton.rect.height;
    endTurnButton.fontSize = 32;

    TextCopy(attackButton.text, "ATTACK");
    attackButton.textColor = WHITE;
    attackButton.buttonColor = RED;
    attackButton.hoverColor = DARKGRAY;
    attackButton.rect.width = 200;
    attackButton.rect.height = 120;
    attackButton.rect.x = GetScreenWidth() / 2.0f - attackButton.rect.width / 2;
    attackButton.rect.y = GetScreenHeight() - attackButton.rect.height * 2;
    attackButton.fontSize = 32;

    BeginTurn();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    UpdateGameCamera(&camera);

    // Level variables
    Vector3 bottomLeft = { 0.0f, 0.0f, 0.0f };
    Vector3 bottomRight = { MAP_WIDTH, 0.0f, 0.0f };
    Vector3 topLeft = { 0.0f, 0.0f, MAP_HEIGHT };
    Vector3 topRight = { MAP_WIDTH, 0.0f, MAP_HEIGHT };

    float boxSize = 1.0f;
    float boxHeight = 0.05f;

    Ray mouseRay = GetMouseRay(GetMousePosition(), camera);

    for (int i = 0; i < numEntities; i++)
    {
        Vector3 orcBoxMin = { entities[i].position.x, entities[i].position.y - boxHeight, entities[i].position.z };
        Vector3 orcBoxMax = { entities[i].position.x + boxSize, entities[i].position.y, entities[i].position.z + boxSize };
        entities[i].boundingBox = (BoundingBox){ orcBoxMin, orcBoxMax };
    }

    hitMapWorld = GetRayCollisionQuad(mouseRay, bottomLeft, topLeft, topRight, bottomRight);

    float selectionRectX = floorf(hitMapWorld.point.x);
    float selectionRectZ = floorf(hitMapWorld.point.z);

    Tile* selectionTile = &tileMap[(int)selectionRectZ][(int)selectionRectX];
    selectionRectPos = (Vector3){ selectionRectX, selectionTile->entityPos, selectionRectZ };

    if (IsButtonClicked(&endTurnButton))
    {
        EndTurn();
    }
    else if (IsButtonClicked(&attackButton) && selection != -1)
    {
        targetingMode = true;
        TextCopy(attackButton.text, "TARGET");
    }
    else if (IsMouseButtonPressed(0) && hitMapWorld.hit)
    {
        if (selection != -1)
        {
            Entity* entity = &entities[selection];

            if (IsTileSelectable(selectionTile))
            {
                // Entity movement
                if (selectionTile->entity == NULL || (entity->target != NULL && selectionTile->entity == entity))
                {
                    entity->position = selectionRectPos;
                    Vector3 orcBoxMin = { entity->position.x, entity->position.y - boxHeight, entity->position.z };
                    Vector3 orcBoxMax = { entity->position.x + boxSize, entity->position.y, entity->position.z + boxSize };
                    entity->boundingBox = (BoundingBox){ orcBoxMin, orcBoxMax };

                    entity->tile->entity = NULL;
                    entity->tile = selectionTile;
                    selectionTile->entity = entity;

                    // Attack
                    if (entity->target != NULL)
                    {
                        int damage = GetRandomValue(entity->minAttack, entity->maxAttack);
                        entity->target->health = entity->target->health - damage;

                        if (entity->target->health <= 0)
                        {
                            entity->target->health = 0;
                            KillEntity(entity->target);
                        }
                    }
                    entity->target = NULL;
                    EndTurn();
                }

                // Entity attack
                else if (IsEnemy(selectionTile->entity) && selectionTile->entity->type == ENTITY_TYPE_CHARACTER)
                {
                    //selectionTile->entity->isAlive = false;

                    // Find tiles where we can hit the enemy.
                    entity->target = selectionTile->entity;
                    Vector3 enemyPos = selectionTile->entity->position;
                    int numAttackTiles = 0;

                    for (int i = 0; i < numSelectionTiles; i++)
                    {
                        float tileDistance = Vector2Distance((Vector2) { enemyPos.x + 0.5f, enemyPos.z + 0.5f }, selectionTiles[i]->tileCenterPos);

                        if (tileDistance < 1.45f) // Replace with attack range?
                        {
                            selectionTiles[numAttackTiles] = selectionTiles[i];
                            numAttackTiles++;
                        }
                    }
                    numSelectionTiles = numAttackTiles;
                }
            }

        }
    }
    if (selection != -1)
    {
        if (IsKeyPressed(KEY_K)) RemoveEntity(&entities[selection]);
        if (IsKeyPressed(KEY_L)) KillEntity(&entities[selection]);
    }
}

void DrawEntityTurnQueue()
{
    int index = 0;

    for (int i = 0; i < numEntityTurns; i++)
    {
        int x = 1700;
        int y = 100;

        if (entityTurnQueue[i] != NULL && entityTurnQueue[i]->isAlive && entityTurnQueue[i]->type == ENTITY_TYPE_CHARACTER)
        {
            DrawText(TextFormat("%d: %s [%d]", index + 1, entityTurnQueue[i]->name, entityTurnQueue[i]->currentInitiative), x, y + index * 30, 20, MAROON);
            index++;
        }
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);

    BeginMode3D(camera);

        DrawTiles(&tileMap[0][0], MAP_HEIGHT, MAP_WIDTH, camera);
        //DrawGameGrid(MAP_WIDTH, MAP_HEIGHT, 1);

        if (selection != -1)
        {
            DrawSelectionArea(selectionTiles, numSelectionTiles, &entities[selection]);
        }

        if (hitMapWorld.hit)
        {
            Color color = { WHITE.r, WHITE.g, WHITE.b, 96 };
            Tile* tile = &tileMap[(int)selectionRectPos.z][(int)selectionRectPos.x];

            Vector3 bottomLeft = tile->bottomLeft;
            bottomLeft.y -= 0.02f;
            Vector3 bottomRight = tile->bottomRight;
            bottomRight.y -= 0.02f;
            Vector3 topRight = tile->topRight;
            topRight.y -= 0.02f;
            Vector3 topLeft = tile->topLeft;
            topLeft.y -= 0.02f;

            DrawQuad3D(camera, bottomLeft, bottomRight, topRight, topLeft, color);
        }

        DrawEntities(entities, numEntities, selection, camera);
        
    EndMode3D();

    if (hitMapWorld.hit)
    {
        DrawText(TextFormat("HIT %.3f | %.3f | %.3f", hitMapWorld.point.x, hitMapWorld.point.y, hitMapWorld.point.z), 130, 200, 20, MAROON);
        //TraceLog(LOG_INFO, "HIT %f | %f | %f", hitMap.point.x, hitMap.point.y, hitMap.point.z);
    }

    // Draw entity turn queue
    DrawEntityTurnQueue();

    Vector2 pos = { 20, 10 };
    DrawTextEx(font, "GAMEPLAY SCREEN", pos, font.baseSize * 3.0f, 4, MAROON);
   
    DrawButton(&endTurnButton);
    DrawButton(&attackButton);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        entities[i] = (Entity){ 0 };
        entityTurnQueue[i] = NULL;
    }
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}