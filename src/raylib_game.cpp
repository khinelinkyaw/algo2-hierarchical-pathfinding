/*******************************************************************************************
*
*   raylib game template
*
*   Hierarchical Pathfinding
*   <Game description>
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "constants.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };

static void UpdateDrawFrame(void);          // Update and draw one frame

int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(Consts::SCREEN_WIDTH, Consts::SCREEN_HEIGHT, "Hierarchical Pathfinding");

    InitAudioDevice();      // Initialize audio device

    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");
    //music = LoadMusicStream("resources/ambient.ogg"); // TODO: Load music
    fxCoin = LoadSound("resources/coin.wav");

    SetMusicVolume(music, 1.0f);
    PlayMusicStream(music);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // Unload global data loaded
    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadSound(fxCoin);

    CloseAudioDevice();     // Close audio context

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawFPS(10, 10);

    EndDrawing();
}
