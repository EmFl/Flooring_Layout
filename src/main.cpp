#include <random>
#include <vector>

#include "config.h"
#include "flooring.h"
#include "plank.h"
#include "raylib.h"

auto main() -> int
{
    InitWindow(WIDTH, HEIGHT, "Planks");

    Vector2 camera_target{ SCREEN_CENTER_X, SCREEN_CENTER_Y };

    Camera2D camera = { 0 };
    camera.target = camera_target;
    camera.offset = (Vector2){ SCREEN_CENTER_X, SCREEN_CENTER_Y };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(TARGET_FPS);

    const auto [planks, index] = calculate({ 560, 400 }, { 130, 25 });

    while (!WindowShouldClose())
    {
        // Move camera
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        {
            camera_target.x += CAMERA_MOVE_SPEED;
        }
        else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        {
            camera_target.x -= CAMERA_MOVE_SPEED;
        }
        else if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        {
            camera_target.y -= CAMERA_MOVE_SPEED;
        }
        else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        {
            camera_target.y += CAMERA_MOVE_SPEED;
        }
        camera.target = camera_target;

        // Camera zoom controls
        camera.zoom += (GetMouseWheelMove() * CAMERA_ZOOM_SCALE);
        if (camera.zoom > CAMERA_ZOOM_MAX)
        {
            camera.zoom = CAMERA_ZOOM_MAX;
        }
        else if (camera.zoom < CAMERA_ZOOM_MIN)
        {
            camera.zoom = CAMERA_ZOOM_MIN;
        }

        // Camera reset (zoom and rotation)
        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
            camera_target = { SCREEN_CENTER_X, SCREEN_CENTER_Y };
            camera.target = camera_target;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        for (const auto& plank : planks)
        {
            plank.draw();
        }

        EndMode2D();

        std::stringstream ss;
        ss << "Planks needed: " << index;

        DrawText(ss.str().c_str(), 10, 130, 20, RED);

        DrawRectangle(10, 10, 250, 113, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(10, 10, 250, 113, BLUE);

        DrawText("Controls:", 20, 20, 10, BLACK);
        DrawText("- WSAD or Arrow keys to move", 40, 40, 10, DARKGRAY);
        DrawText("- Mouse Wheel to Zoom in-out", 40, 60, 10, DARKGRAY);
        DrawText("- R to reset Zoom and Position", 40, 80, 10, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}