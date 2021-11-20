#include <iomanip>
#include <random>
#include <string>
#include <vector>

#include "config.h"
#include "flooring.h"
#include "plank.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "extras/raygui.h"

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

    std::pair<int, int> room_size{ 560, 400 };
    std::pair<int, int> plank_size{ 130, 25 };

    auto result = calculate(room_size, plank_size);

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

        for (const auto& plank : result.planks)
        {
            plank.draw();
        }

        EndMode2D();

        static constexpr auto blue_rect_height = 360;

        DrawRectangle(10, 10, 300, blue_rect_height, { 167u, 199u, 231u, 255u });
        DrawRectangleLines(10, 10, 300, blue_rect_height, BLUE);

        DrawText("Controls:", 20, 20, 10, BLACK);
        DrawText("- WSAD or Arrow keys to move", 40, 40, 10, BLACK);
        DrawText("- Mouse Wheel to Zoom in-out", 40, 60, 10, BLACK);
        DrawText("- R to reset Zoom and Position", 40, 80, 10, BLACK);

        static constexpr std::pair<int, int> room_range{ 20, 81 };
        static constexpr std::pair<int, int> plank_range{ 1, 31 };
        static constexpr int multiplier = 10;

        room_size.first = std::floor(GuiSliderBar(
                              (Rectangle){ 80, 100, 120, 20 },
                              "Room X",
                              std::to_string(room_size.first).c_str(),
                              static_cast<float>(room_size.first / multiplier),
                              room_range.first,
                              room_range.second)) *
                          multiplier;
        room_size.second = std::floor(GuiSliderBar(
                               (Rectangle){ 80, 130, 120, 20 },
                               "Room Y",
                               std::to_string(room_size.second).c_str(),
                               static_cast<float>(room_size.second / multiplier),
                               room_range.first,
                               room_range.second)) *
                           multiplier;

        plank_size.first = std::floor(GuiSliderBar(
                               (Rectangle){ 80, 160, 120, 20 },
                               "Plank X",
                               std::to_string(plank_size.first).c_str(),
                               static_cast<float>(plank_size.first / multiplier),
                               plank_range.first,
                               plank_range.second)) *
                           multiplier;
        plank_size.second = std::floor(GuiSliderBar(
                                (Rectangle){ 80, 190, 120, 20 },
                                "Plank Y",
                                std::to_string(plank_size.second).c_str(),
                                static_cast<float>(plank_size.second / multiplier),
                                plank_range.first,
                                plank_range.second)) *
                            multiplier;

        std::stringstream ss;
        ss << "Planks needed: " << result.all_planks << '\n'
           << "Left over pieces: " << result.left_over << '\n'
           << "Uncut planks: " << result.uncut;
        DrawText(ss.str().c_str(), 40, 220, 20, BLACK);

        if (GuiButton((Rectangle){ 40, 320, 120, 30 }, "RECALCULATE"))
        {
            result = calculate(room_size, plank_size);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}