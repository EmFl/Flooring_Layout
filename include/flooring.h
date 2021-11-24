#pragma once

#include <raylib.h>

#include <algorithm>
#include <array>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "plank.h"

inline constexpr size_t STAGGER_PATTERN_SIZE = 6;
inline constexpr std::array<int, STAGGER_PATTERN_SIZE> STAGGER_PATTERN{ { 0, 50, -30, 30, -20, 20 } };

struct Result
{
    int all_planks;
    int left_over;
    int uncut;
    std::vector<Plank> planks;
    std::vector<Plank> left_over_pieces;
};

class Flooring
{
public:
    void configure(std::pair<int, int>& room_size, std::pair<int, int>& plank_size, bool staggered, bool randomize_lengths)
    {
        room_size_ = room_size;
        plank_size_ = plank_size;
        staggered_ = staggered;
        randomize_lengths_ = randomize_lengths;
    }

    [[nodiscard]] auto calculate() -> Result;

private:
    std::pair<int, int> room_size_{ 0, 0 };
    std::pair<int, int> plank_size_{ 0, 0 };
    bool staggered_ = false;
    bool randomize_lengths_ = false;

    std::pair<int, int> current_position_{ 0, 0 };
    size_t stagger_pattern_index_ = 0;

    std::vector<Plank> planks_{};
    std::vector<Plank> left_over_pieces_{};

    std::mt19937 rng_{ std::random_device{}() };

    [[nodiscard]] auto generate_color(int index) -> Color;
    [[nodiscard]] auto generate_lengths(int start, int end) -> int;
    [[nodiscard]] auto calculate_slice() -> std::pair<int, int>;

    void increment_position(const std::pair<int, int>& offset);

    void cleanup_left_over_pieces();
    void position_left_over_pieces();

    [[nodiscard]] auto find_and_place_usable_piece(
        bool is_sliced_horizontally,
        bool is_sliced_vertically,
        const std::pair<int, int>& size_lookup) -> bool;
};
