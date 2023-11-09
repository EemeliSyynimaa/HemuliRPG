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

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
#define MAP_WIDTH 20
#define MAP_HEIGHT 15

#define MAP_HEIGHT_VERTICES MAP_HEIGHT + 1
#define MAP_WIDTH_VERTICES MAP_WIDTH + 1
#define TILE_SIZE 1

#define MAX_ENTITIES 10

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

void DrawEntities(Entity entities[], int numEntities, int selected, Camera camera)
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
        if (renderQueue[i] == selected)
        {
            Color color = { YELLOW.r, YELLOW.g, YELLOW.b, 96 };
            DrawRectangle3D(camera, entities[renderQueue[i]].position, (Vector2) { 1.0f, 1.0f }, color);
        }

        Vector3 up = { 0.0f, -1.0f, 0.0f };
        Vector2 origin = Vector2Zero();
        float rotation = 0.0f;
        Color tint = WHITE;

        Entity* entity = &entities[renderQueue[i]];
        Vector3 entityPos = entity->position;

        // Render separate from map position.
        entityPos.x += 0.5f;
        entityPos.y += -0.5f;
        entityPos.z += 0.5f;

        DrawBillboardPro(camera, entity->texture, entity->textureRect, entityPos, up, entity->size, origin, rotation, tint);

        DrawBoundingBox(entities[renderQueue[i]].boundingBox, WHITE);
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

    // Camera movement
    if (!IsGamepadAvailable(0))
    {
        // Mouse support when right clicked.
        if (IsMouseButtonDown(1))
        {
            CameraYaw(camera, -mousePositionDelta.x * cameraMouseMoveSensitivity, rotateAroundTarget);
            CameraPitch(camera, -mousePositionDelta.y * cameraMouseMoveSensitivity, lockView, rotateAroundTarget, rotateUp);
        }

        // Keyboard support
        if (IsKeyDown(KEY_W)) CameraMoveForward(camera, cameraMoveSpeed, moveInWorldPlane);
        if (IsKeyDown(KEY_A)) CameraMoveRight(camera, -cameraMoveSpeed, moveInWorldPlane);
        if (IsKeyDown(KEY_S)) CameraMoveForward(camera, -cameraMoveSpeed, moveInWorldPlane);
        if (IsKeyDown(KEY_D)) CameraMoveRight(camera, cameraMoveSpeed, moveInWorldPlane);
    }
    else
    {
        // Gamepad controller support
        CameraYaw(camera, -(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 2) * cameraMouseMoveSensitivity, rotateAroundTarget);
        CameraPitch(camera, -(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 2) * cameraMouseMoveSensitivity, lockView, rotateAroundTarget, rotateUp);

        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) <= -0.25f) CameraMoveForward(camera, cameraMoveSpeed, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) <= -0.25f) CameraMoveRight(camera, -cameraMoveSpeed, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) >= 0.25f) CameraMoveForward(camera, -cameraMoveSpeed, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) >= 0.25f) CameraMoveRight(camera, cameraMoveSpeed, moveInWorldPlane);
    }

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

static Tile tileMap[MAP_HEIGHT][MAP_WIDTH];
static float depthMap[MAP_HEIGHT_VERTICES][MAP_WIDTH_VERTICES];

Entity entities[MAX_ENTITIES] = { 0 };
RayCollision hitMapWorld = { 0 };
Vector3 selectionRectPos = { 0 };

int selection = -1;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
void CreateOrc(int entityIndex)
{
    entities[entityIndex].position = (Vector3){ 1.0f, 0.0f, 1.0f + entityIndex};
    entities[entityIndex].size = (Vector2){ 1.0f, 1.0f };
    entities[entityIndex].texture = orcTexture;
    entities[entityIndex].textureRect = (Rectangle){ 0.0f, 0.0f, (float)orcTexture.width, (float)orcTexture.height };
}

void CreateWizard(int entityIndex)
{
    entities[entityIndex].position = (Vector3){ 10.0f, 0.0f, 1.0f + entityIndex };
    entities[entityIndex].size = (Vector2){ 1.0f, 1.0f };
    entities[entityIndex].texture = wizardTexture;
    entities[entityIndex].textureRect = (Rectangle){ 0.0f, 0.0f, (float)wizardTexture.width, (float)wizardTexture.height };
}


// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;

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

            tile->entityPos = (depthMap[z][x] + depthMap[z][x + 1] + depthMap[z + 1][x + 1] + depthMap[z + 1][x]) / 4;
            tile->texture = grassTexture;
        }
    }

    // Initialize Entities
    for (int i = 0; i < MAX_ENTITIES / 2; i++)
    {
        CreateOrc(i);
    }    
    for (int i = MAX_ENTITIES / 2; i < MAX_ENTITIES; i++)
    {
        CreateWizard(i);
    }
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

    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        Vector3 orcBoxMin = { entities[i].position.x, entities[i].position.y - boxHeight, entities[i].position.z };
        Vector3 orcBoxMax = { entities[i].position.x + boxSize, entities[i].position.y, entities[i].position.z + boxSize };
        entities[i].boundingBox = (BoundingBox){ orcBoxMin, orcBoxMax };
    }

    hitMapWorld = GetRayCollisionQuad(mouseRay, bottomLeft, topLeft, topRight, bottomRight);

    float selectionRectX = floorf(hitMapWorld.point.x);
    float selectionRectZ = floorf(hitMapWorld.point.z);
    selectionRectPos = (Vector3){ selectionRectX, tileMap[(int)selectionRectZ][(int)selectionRectX].entityPos, selectionRectZ};

    if (IsMouseButtonPressed(0))
    {
        bool entityPicked = false;

        for (int i = 0; i < MAX_ENTITIES; i++)
        {
            RayCollision hitMapEntity = GetRayCollisionBox(mouseRay, entities[i].boundingBox);

            if (hitMapEntity.hit)
            {
                hitMapEntity.point = entities[i].position;
                DrawText(TextFormat("ORC HIT %.3f | %.3f | %.3f", hitMapEntity.point.x, hitMapEntity.point.y, hitMapEntity.point.z), 130, 180, 20, MAROON);
                //TraceLog(LOG_INFO, "HIT %f | %f | %f", hitMap.point.x, hitMap.point.y, hitMap.point.z);
                selection = i;
                entityPicked = true;
            }
        }

        if (!entityPicked && selection != -1 && hitMapWorld.hit)
        {
            Entity* entity = &entities[selection];
            entity->position = selectionRectPos;
            Vector3 orcBoxMin = { entity->position.x, entity->position.y - boxHeight, entity->position.z };
            Vector3 orcBoxMax = { entity->position.x + boxSize, entity->position.y, entity->position.z + boxSize };
            entity->boundingBox = (BoundingBox){ orcBoxMin, orcBoxMax };
            selection = -1;
        }
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    Vector2 pos = { 20, 10 };
    DrawTextEx(font, "GAMEPLAY SCREEN", pos, font.baseSize*3.0f, 4, MAROON);
    DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);

    BeginMode3D(camera);

        DrawTiles(&tileMap[0][0], MAP_HEIGHT, MAP_WIDTH, camera);
        //DrawGameGrid(MAP_WIDTH, MAP_HEIGHT, 1);

        if (hitMapWorld.hit)
        {
            Color color = { WHITE.r, WHITE.g, WHITE.b, 96 };
            Tile* tile = &tileMap[(int)selectionRectPos.z][(int)selectionRectPos.x];

            DrawQuad3D(camera, tile->bottomLeft, tile->bottomRight, tile->topRight, tile->topLeft, color);
        }

        DrawEntities(entities, MAX_ENTITIES, selection, camera);
        

    EndMode3D();

    if (hitMapWorld.hit)
    {
        DrawText(TextFormat("HIT %.3f | %.3f | %.3f", hitMapWorld.point.x, hitMapWorld.point.y, hitMapWorld.point.z), 130, 200, 20, MAROON);
        //TraceLog(LOG_INFO, "HIT %f | %f | %f", hitMap.point.x, hitMap.point.y, hitMap.point.z);
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}