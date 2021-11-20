#pragma once

#include <vector>

#include "plank.h"

struct calculation_result
{
    int all_planks{};
    int left_over{};
    int uncut{};
    std::vector<Plank> planks{};
};

auto calculate(std::pair<int, int> room_size, std::pair<int, int> plank_size, bool staggered) -> calculation_result;
