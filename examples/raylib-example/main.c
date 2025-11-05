/*******************************************************************************************
 *
 *   raylib [shapes] example - bouncing ball
 *
 *   Example complexity rating: [★☆☆☆] 1/4
 *
 *   Example originally created with raylib 2.5, last time updated with
 *   raylib 2.5
 *
 *   Example contributed by Ramon Santamaria (@raysan5), reviewed by Jopestpe
 *   (@jopestpe)
 *
 *   Example licensed under an unmodified zlib/libpng license, which is an
 *   OSI-certified, BSD-like license that allows static linking with closed
 *   source software
 *
 *   Copyright (c) 2013-2025 Ramon Santamaria (@raysan5)
 *   Modified by Colan Biemer as an example of adjusting
 *
 ********************************************************************************************/

#include "adjust.h"
#include "raylib.h" // m

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
// "// m" marks a modification made to the example // m
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    adjust_init(); // m
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight,
               "raylib [shapes] example - bouncing ball");

    Vector2 ballPosition = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    Vector2 ballSpeed = {5.0f, 4.0f};
    ADJUST_CONST_INT(ballRadius, 10);  // m
    ADJUST_CONST_FLOAT(gravity, 0.2f); // m

    bool useGravity = true;
    bool pause = 0;
    int framesCounter = 0;

    // m
    ADJUST_CONST_STRING(spaceString, "PRESS SPACE to PAUSE BALL MOVEMENT");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //-----------------------------------------------------
        if (IsKeyPressed(KEY_G))
            useGravity = !useGravity;
        if (IsKeyPressed(KEY_SPACE))
            pause = !pause;
        if (IsKeyPressed(KEY_R)) // m
            adjust_update();     // m

        if (!pause)
        {
            ballPosition.x += ballSpeed.x;
            ballPosition.y += ballSpeed.y;

            if (useGravity)
                ballSpeed.y += gravity;

            // Check walls collision for bouncing
            if ((ballPosition.x >= (GetScreenWidth() - ballRadius)) ||
                (ballPosition.x <= ballRadius))
                ballSpeed.x *= -1.0f;
            if ((ballPosition.y >= (GetScreenHeight() - ballRadius)) ||
                (ballPosition.y <= ballRadius))
                ballSpeed.y *= -0.95f;
        }
        else
            framesCounter++;
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawCircleV(ballPosition, (float)ballRadius, MAROON);
        DrawText(spaceString, 10, GetScreenHeight() - 25, 20, LIGHTGRAY); // m

        if (useGravity)
            DrawText("GRAVITY: ON (Press G to disable)", 10,
                     GetScreenHeight() - 50, 20, DARKGREEN);
        else
            DrawText("GRAVITY: OFF (Press G to enable)", 10,
                     GetScreenHeight() - 50, 20, RED);

        // On pause, we draw a blinking message
        if (pause && ((framesCounter / 30) % 2))
            DrawText("PAUSED", 350, 200, 30, GRAY);

        DrawFPS(10, 10);

        EndDrawing();
        //-----------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------
    CloseWindow();    // Close window and OpenGL context
    adjust_cleanup(); // m
    //----------------------------------------------------------

    return 0;
}
