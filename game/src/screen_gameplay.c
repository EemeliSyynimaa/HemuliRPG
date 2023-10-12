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

void DrawGrass(Camera camera, Texture2D texture, Vector3 position, Vector2 size, Color tint)
{
    Vector3 topLeft = Vector3Add(position, (Vector3) { 0, 0, size.y });
    Vector3 topRight = Vector3Add(position, (Vector3) { size.x, 0, size.y });
    Vector3 bottomRight = Vector3Add(position, (Vector3) { size.x, 0, 0 });
    Vector3 bottomLeft = position;

    rlSetTexture(texture.id);

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

    rlSetTexture(0);
}

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
#define MAP_WIDTH 20
#define MAP_HEIGHT 15

#define TILE_WIDTH 32
#define TILE_HEIGHT 32

static int framesCounter = 0;
static int finishScreen = 0;
static int tileMap[MAP_HEIGHT][MAP_WIDTH];

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            tileMap[y][x] = GetRandomValue(0, 1);
        }
    }
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!

    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        // finishScreen = 1;
        // PlaySound(fxCoin);
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    Vector2 pos = { 20, 10 };
    DrawTextEx(font, "GAMEPLAY SCREEN", pos, font.baseSize*3.0f, 4, MAROON);
    DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);

    UpdateCamera(&camera, CAMERA_THIRD_PERSON);

    BeginMode3D(camera);
        for (int z = 0; z < MAP_HEIGHT; z++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                Vector3 position = { (float)x, 0, (float)z };
                Vector2 size = { 1.0f, 1.0f };

                DrawGrass(camera, grass, position, size, WHITE);
            }
        }

        Vector2 size = { 1.0f, 1.0f };
        Rectangle rekt = { 0.0f, 0.0f, orc.width, orc.height };

        DrawBillboardPro(camera, orc, rekt, (Vector3) { 1.5f, -0.5f, 1.5f }, (Vector3) { 0.0f, -1.0f, 0.0f }, size, Vector2Zero(), 0.0f, WHITE);

    EndMode3D();
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