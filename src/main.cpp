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

    bool staggered = true;
    bool randomize = false;

    std::pair<int, int> room_size{ 560, 400 };  // NOLINT
    std::pair<int, int> plank_size{ 130, 25 };  // NOLINT

    // config sliders
    static constexpr std::pair<int, int> room_range{ 200, 801 };
    static constexpr std::pair<int, int> plank_x_range{ 60, 301 };
    static constexpr std::pair<int, int> plank_y_range{ 10, 100 };

    Flooring flooring;
    flooring.configure(room_size, plank_size, staggered, randomize);
    auto result = flooring.calculate();

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

        // Increase / Decrease room size
        // U I O P - fine-tune room size
        if (IsKeyPressed(KEY_U))
        {
            room_size.first = std::max(room_range.first, room_size.first - 1);
        }
        if (IsKeyPressed(KEY_I))
        {
            room_size.first = std::min(room_range.second, room_size.first + 1);
        }
        if (IsKeyPressed(KEY_O))
        {
            room_size.second = std::max(room_range.first, room_size.second - 1);
        }
        if (IsKeyPressed(KEY_P))
        {
            room_size.second = std::min(room_range.second, room_size.second + 1);
        }

        // Increase / Decrease plank size
        // H J K L - fine-tune plank size
        if (IsKeyPressed(KEY_H))
        {
            plank_size.first = std::max(plank_x_range.first, plank_size.first - 1);
        }
        if (IsKeyPressed(KEY_J))
        {
            plank_size.first = std::min(plank_x_range.second, plank_size.first + 1);
        }
        if (IsKeyPressed(KEY_K))
        {
            plank_size.second = std::max(plank_y_range.first, plank_size.second - 1);
        }
        if (IsKeyPressed(KEY_L))
        {
            plank_size.second = std::min(plank_y_range.second, plank_size.second + 1);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        for (const auto& plank : result.planks)
        {
            plank.draw();
        }
        for (const auto& piece : result.left_over_pieces)
        {
            piece.draw();
        }

        EndMode2D();

        static constexpr auto blue_rect_height = 460;

        // background
        DrawRectangle(10, 10, 300, blue_rect_height, { 167u, 199u, 231u, 255u });
        DrawRectangleLines(10, 10, 300, blue_rect_height, BLUE);

        // info
        DrawText("Controls:", 20, 20, 10, BLACK);
        DrawText("- WSAD or Arrow keys to move", 40, 40, 10, BLACK);
        DrawText("- Mouse Wheel to Zoom in-out", 40, 60, 10, BLACK);
        DrawText("- R to reset Zoom and Position", 40, 80, 10, BLACK);
        DrawText("- UIOP to fine-tune room size", 40, 100, 10, BLACK);
        DrawText("- HJKL to fine-tune plank size", 40, 120, 10, BLACK);

        room_size.first = std::floor(GuiSliderBar(
            (Rectangle){ 80, 140, 120, 20 },
            "Room X",
            std::to_string(room_size.first).c_str(),
            static_cast<float>(room_size.first),
            room_range.first,
            room_range.second));
        room_size.second = std::floor(GuiSliderBar(
            (Rectangle){ 80, 170, 120, 20 },
            "Room Y",
            std::to_string(room_size.second).c_str(),
            static_cast<float>(room_size.second),
            room_range.first,
            room_range.second));

        plank_size.first = std::floor(GuiSliderBar(
            (Rectangle){ 80, 200, 120, 20 },
            "Plank X",
            std::to_string(plank_size.first).c_str(),
            static_cast<float>(plank_size.first),
            plank_x_range.first,
            plank_x_range.second));
        plank_size.second = std::floor(GuiSliderBar(
            (Rectangle){ 80, 230, 120, 20 },
            "Plank Y",
            std::to_string(plank_size.second).c_str(),
            static_cast<float>(plank_size.second),
            plank_y_range.first,
            plank_y_range.second));

        // display calculation results
        std::stringstream ss;
        ss << "Planks needed: " << result.all_planks << '\n'
           << "Left over pieces: " << result.left_over << '\n'
           << "Uncut planks: " << result.uncut;
        DrawText(ss.str().c_str(), 40, 270, 20, BLACK);

        // checkboxes
        staggered = GuiCheckBox((Rectangle){ 40, 370, 20, 20 }, "Stagger Pattern", staggered);
        randomize = GuiCheckBox((Rectangle){ 40, 400, 20, 20 }, "Randomize Lengths", randomize);

        // Recalculate button
        if (GuiButton((Rectangle){ 40, 430, 120, 30 }, "RECALCULATE"))
        {
            flooring.configure(room_size, plank_size, staggered, randomize);
            result = flooring.calculate();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}