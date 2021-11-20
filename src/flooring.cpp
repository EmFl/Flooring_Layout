#include "flooring.h"

#include <raylib.h>

#include <random>
#include <vector>

#include "plank.h"

auto generate_color(int index) -> Color
{
    static constexpr auto color_max_value = 255u;
    static constexpr auto color_min_value = 100u;

    static std::random_device dev;
    static std::mt19937 rng(dev());
    static std::uniform_int_distribution<unsigned char> dist(color_min_value, color_max_value);
    return { dist(rng), dist(rng), dist(rng), color_max_value };
}

auto calculate(std::pair<int, int> room_size, std::pair<int, int> plank_size, bool staggered) -> calculation_result
{
    static const std::vector<int> stagger_pattern{ 0, 40, plank_size.first - 40, 20 };

    std::vector<Plank> planks;
    std::vector<Plank> left_over_pieces;

    int index = 0;
    int uncut_planks = 0;

    int y = 0;
    int x = 0;

    int stagger_pattern_index = 0;

    while (y < room_size.second && x < room_size.first)
    {
        // check the size of the slice
        int slice_right = 0;
        if (x == 0)
        {
            slice_right = stagger_pattern.at(stagger_pattern_index);
        }
        else if ((x + plank_size.first) > room_size.first)
        {
            slice_right = room_size.first - x;
        }

        // if the plank should be cut vertically
        const bool is_sliced_vertically = slice_right > 0;

        int slice_bottom = 0;
        if ((y + plank_size.second) > room_size.second)
        {
            slice_bottom = room_size.second - y;
        }

        // if the plank should be cut horizontally
        const bool is_sliced_horizontally = slice_bottom > 0;

        const std::pair<int, int> plank_position = { x, y };

        if (!is_sliced_horizontally && !is_sliced_vertically)
        {
            index++;
            uncut_planks++;
            planks.emplace_back(index, plank_position, plank_size, generate_color(index));
            x += plank_size.first;
            if (x >= room_size.first)
            {
                y += plank_size.second;
                x = 0;
                if (staggered)
                {
                    stagger_pattern_index++;
                    if (stagger_pattern_index > stagger_pattern.size() - 1)
                    {
                        stagger_pattern_index = 0;
                    }
                }
            }
            continue;
        }

        // dimensions we are looking for
        std::pair<int, int> size_lookup{ is_sliced_vertically ? slice_right : plank_size.first,
                                         is_sliced_horizontally ? slice_bottom : plank_size.second };

        // check if we have a usable plank piece in the left overs
        bool found = false;
        for (auto &lo : left_over_pieces)
        {
            if (lo.dimensions_ >= size_lookup)
            {
                // slice off the part that is needed
                if (is_sliced_horizontally)
                {
                    lo.dimensions_.second -= size_lookup.second;
                }
                if (is_sliced_vertically)
                {
                    lo.dimensions_.first -= size_lookup.first;
                }
                // store the sliced of part
                planks.emplace_back(lo.id_, plank_position, size_lookup, lo.color_);
                found = true;
                break;
            }
        }

        // there is no usable slice in left overs, get a new plank
        if (!found)
        {
            const auto color = generate_color(index);
            // make a new plank
            index++;
            planks.emplace_back(index, plank_position, size_lookup, color);
            // store the remaining part in the left overs

            const std::pair<int, int> left_over_dimensions = {
                is_sliced_vertically ? plank_size.first - size_lookup.first : plank_size.first,
                is_sliced_horizontally ? plank_size.second - size_lookup.second : plank_size.second
            };
            const std::pair<int, int> empty_pos{ 0, 0 };
            left_over_pieces.emplace_back(index, empty_pos, left_over_dimensions, color);
        }

        // cleanup - erase all pieces with a dimension of 0
        left_over_pieces.erase(
            std::remove_if(
                left_over_pieces.begin(),
                left_over_pieces.end(),
                [](auto &p) { return p.dimensions_.first <= 0 || p.dimensions_.second <= 0; }),
            left_over_pieces.end());

        // increment x and y
        x += size_lookup.first;
        if (x >= room_size.first)
        {
            y += size_lookup.second;
            x = 0;
            if (staggered)
            {
                stagger_pattern_index++;
                if (stagger_pattern_index > stagger_pattern.size() - 1)
                {
                    stagger_pattern_index = 0;
                }
            }
        }
    }

    // lay out the unused pieces below the grid
    int left_over_col = 0;
    int left_over_row = 1;
    static constexpr auto left_over_offset = 10;
    for (auto &lo : left_over_pieces)
    {
        lo.position_ = { left_over_col * (plank_size.first + left_over_offset),
                         room_size.second + (left_over_row * (plank_size.second + left_over_offset)) };
        left_over_col++;
        if (left_over_col > 4)
        {
            left_over_col = 0;
            left_over_row++;
        }
    }

    // append all planks together
    planks.insert(planks.end(), left_over_pieces.begin(), left_over_pieces.end());

    calculation_result result;
    result.all_planks = index;
    result.left_over = static_cast<int>(left_over_pieces.size());
    result.uncut = uncut_planks;
    result.planks = planks;
    return result;
}
