#ifndef PLANK_H
#define PLANK_H

#include <sstream>
#include <string>
#include <utility>

#include "raylib.h"

struct Plank
{
    int id_;
    std::pair<int, int> position_;
    std::pair<int, int> dimensions_;
    Color color_;

    explicit Plank(int identifier, std::pair<int, int> position, std::pair<int, int> dimensions, Color color)
        : id_(identifier),
          position_(std::move(position)),
          dimensions_(std::move(dimensions)),
          color_(color){};

    auto draw() const -> void
    {
        static constexpr auto fontSize = 24;
        static constexpr auto drawScaleFactor = 4;
        static constexpr auto textOffset = 6;

        const auto x = position_.first * drawScaleFactor;
        const auto y = position_.second * drawScaleFactor;
        const auto w = dimensions_.first * drawScaleFactor;
        const auto h = dimensions_.second * drawScaleFactor;

        DrawRectangle(x, y, w, h, color_);
        std::stringstream ss;
        ss << '(' << id_ << ") " << dimensions_.first << " x " << dimensions_.second;
        DrawText(ss.str().c_str(), x + textOffset, y + textOffset, fontSize, DARKGRAY);
        DrawRectangleLines(x, y, w, h, BLACK);
    }
};

#endif
