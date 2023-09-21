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

    static float hehe = 0.0f;

    BeginMode3D(camera);
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                hehe += 0.1f;
                //DrawBillboard(camera, grass, (Vector3) { x, y, 0 }, 1, WHITE);
                //DrawTexture(grass, x * TILE_WIDTH, y * TILE_HEIGHT, WHITE);
                //DrawPlane((Vector3) { x, y, 0 }, (Vector2){ 1, 1 }, WHITE);
                Rectangle source = { 0.0f, 0.0f, (float)grass.width, (float)grass.height };
                Vector3 up = { 0.0f, 1.0f, 0.0f };
                Vector3 position = { x, y, 0 };
                Vector2 size = { 1.0f, 1.0f };
                Vector2 origin = { 0.0f, 0.0f };
                DrawBillboardPro(camera, grass, source, position, up, size, origin, hehe, WHITE);
            }

        }
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