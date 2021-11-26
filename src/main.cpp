#include <iomanip>
#include <random>
#include <string>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>

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

    Camera2D camera{};
    camera.target = camera_target;
    camera.offset = { SCREEN_CENTER_X, SCREEN_CENTER_Y };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(TARGET_FPS);

    bool staggered = true;
    bool randomize = false;

    std::pair<int, int> room_size{DEFAULT_ROOM_SIZE};
    std::pair<int, int> plank_size{DEFAULT_PLANK_SIZE};

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
            room_size.first = std::max(SLIDER_ROOM_RANGE.first, room_size.first - 1);
        }
        if (IsKeyPressed(KEY_I))
        {
            room_size.first = std::min(SLIDER_ROOM_RANGE.second, room_size.first + 1);
        }
        if (IsKeyPressed(KEY_O))
        {
            room_size.second = std::max(SLIDER_ROOM_RANGE.first, room_size.second - 1);
        }
        if (IsKeyPressed(KEY_P))
        {
            room_size.second = std::min(SLIDER_ROOM_RANGE.second, room_size.second + 1);
        }

        // Increase / Decrease plank size
        // H J K L - fine-tune plank size
        if (IsKeyPressed(KEY_H))
        {
            plank_size.first = std::max(SLIDER_PLANK_X_RANGE.first, plank_size.first - 1);
        }
        if (IsKeyPressed(KEY_J))
        {
            plank_size.first = std::min(SLIDER_PLANK_X_RANGE.second, plank_size.first + 1);
        }
        if (IsKeyPressed(KEY_K))
        {
            plank_size.second = std::max(SLIDER_PLANK_Y_RANGE.first, plank_size.second - 1);
        }
        if (IsKeyPressed(KEY_L))
        {
            plank_size.second = std::min(SLIDER_PLANK_Y_RANGE.second, plank_size.second + 1);
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
        static constexpr auto blue_rect_width = 300;
        static constexpr Color rect_color = { 167u, 199u, 231u, 255u };
        static constexpr auto blue_rect_pos_x = 10;
        static constexpr auto blue_rect_pos_y = 10;

        // background
        DrawRectangle(blue_rect_pos_x, blue_rect_pos_y, blue_rect_width, blue_rect_height, rect_color);
        DrawRectangleLines(blue_rect_pos_x, blue_rect_pos_y, blue_rect_width, blue_rect_height, BLUE);

        // info
        static constexpr auto text_x = 40;
        static constexpr auto standard_text_font_size = 10;

        DrawText("Controls:", text_x, 20, standard_text_font_size, BLACK);                        // NOLINT
        DrawText("- WSAD or Arrow keys to move", text_x, 40, standard_text_font_size, BLACK);     // NOLINT
        DrawText("- Mouse Wheel to Zoom in-out", text_x, 60, standard_text_font_size, BLACK);     // NOLINT
        DrawText("- R to reset Zoom and Position", text_x, 80, standard_text_font_size, BLACK);   // NOLINT
        DrawText("- UIOP to fine-tune room size", text_x, 100, standard_text_font_size, BLACK);   // NOLINT
        DrawText("- HJKL to fine-tune plank size", text_x, 120, standard_text_font_size, BLACK);  // NOLINT

        // sliders
        static constexpr auto slider_left_margin = 80;
        static constexpr auto slider_width = 120;
        static constexpr auto slider_height = 20;

        static constexpr Rectangle slider1_rect{ slider_left_margin, 140, slider_width, slider_height };
        static constexpr Rectangle slider2_rect{ slider_left_margin, 170, slider_width, slider_height };
        static constexpr Rectangle slider3_rect{ slider_left_margin, 200, slider_width, slider_height };
        static constexpr Rectangle slider4_rect{ slider_left_margin, 230, slider_width, slider_height };

        room_size.first = std::floor(GuiSliderBar(
            slider1_rect,
            "Room X",
            std::to_string(room_size.first).c_str(),
            static_cast<float>(room_size.first),
            static_cast<float>(SLIDER_ROOM_RANGE.first),
            static_cast<float>(SLIDER_ROOM_RANGE.second)));
        room_size.second = std::floor(GuiSliderBar(
            slider2_rect,
            "Room Y",
            std::to_string(room_size.second).c_str(),
            static_cast<float>(room_size.second),
            static_cast<float>(SLIDER_ROOM_RANGE.first),
            static_cast<float>(SLIDER_ROOM_RANGE.second)));
        plank_size.first = std::floor(GuiSliderBar(
            slider3_rect,
            "Plank X",
            std::to_string(plank_size.first).c_str(),
            static_cast<float>(plank_size.first),
            static_cast<float>(SLIDER_PLANK_X_RANGE.first),
            static_cast<float>(SLIDER_PLANK_X_RANGE.second)));
        plank_size.second = std::floor(GuiSliderBar(
            slider4_rect,
            "Plank Y",
            std::to_string(plank_size.second).c_str(),
            static_cast<float>(plank_size.second),
            static_cast<float>(SLIDER_PLANK_Y_RANGE.first),
            static_cast<float>(SLIDER_PLANK_Y_RANGE.second)));

        // display calculation results
        std::stringstream ss;
        ss << "Planks needed: " << result.all_planks << '\n'
           << "Left over pieces: " << result.left_over << '\n'
           << "Uncut planks: " << result.uncut;
        static constexpr auto result_text_size = 20;
        static constexpr std::pair<int, int> result_text_position{ 40, 270 };
        DrawText(ss.str().c_str(), result_text_position.first, result_text_position.second, result_text_size, BLACK);

        // checkboxes
        static constexpr Rectangle checkbox1_rect{ 40, 370, 20, 20 };
        static constexpr Rectangle checkbox2_rect{ 40, 400, 20, 20 };
        staggered = GuiCheckBox((Rectangle)checkbox1_rect, "Stagger Pattern", staggered);
        randomize = GuiCheckBox((Rectangle)checkbox2_rect, "Randomize Lengths", randomize);

        // Recalculate button
        static constexpr Rectangle recalculate_button_rect{ 40, 430, 120, 30 };
        if (GuiButton(recalculate_button_rect, "RECALCULATE"))
        {
            flooring.configure(room_size, plank_size, staggered, randomize);
            result = flooring.calculate();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
