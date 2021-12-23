#include "flooring.h"

#include "random_generator.h"

auto Flooring::generate_color() -> Color
{
    static constexpr auto min_value = 100;
    static constexpr auto max_value = 255;

    return { static_cast<unsigned char>(random_pick(min_value, max_value)),
             static_cast<unsigned char>(random_pick(min_value, max_value)),
             static_cast<unsigned char>(random_pick(min_value, max_value)),
             max_value };
}

auto Flooring::generate_lengths(int start, int end) -> int
{
    return random_pick(start, end);
}

auto Flooring::calculate_slice() -> std::pair<int, int>
{
    std::pair<int, int> slice{ 0, 0 };

    if (current_position_.first == 0 && staggered_)
    {
        const auto stagger_value = STAGGER_PATTERN.at(stagger_pattern_index_);
        slice.first = (stagger_value < 0) ? plank_size_.first + stagger_value : stagger_value;
    }
    else if ((current_position_.first + plank_size_.first) > room_size_.first)
    {
        slice.first = room_size_.first - current_position_.first;
    }
    else if (randomize_lengths_)
    {
        static constexpr auto min_random_slice = 40;
        slice.first = generate_lengths(min_random_slice, plank_size_.first);

        // clamp the random value
        if ((current_position_.first + slice.first) > room_size_.first)
        {
            slice.first = slice.first - ((current_position_.first + slice.first) - room_size_.first);
        }
    }

    // check the row length again
    if ((current_position_.first + slice.first) > room_size_.first)
    {
        throw std::length_error("Error: Row width exceeds room size.");
    }

    if ((current_position_.second + plank_size_.second) > room_size_.second)
    {
        slice.second = room_size_.second - current_position_.second;
    }

    // check the col length again
    if ((current_position_.second + slice.second) > room_size_.second)
    {
        throw std::length_error("Error: Column height exceeds room size.");
    }

    return slice;
}
void Flooring::increment_position(const std::pair<int, int> &offset)
{
    current_position_.first += offset.first;
    if (current_position_.first >= room_size_.first)
    {
        current_position_.second += offset.second;
        current_position_.first = 0;
        if (staggered_)
        {
            stagger_pattern_index_++;
            if (stagger_pattern_index_ > STAGGER_PATTERN_SIZE - 1)
            {
                stagger_pattern_index_ = 0;
            }
        }
    }
}

void Flooring::cleanup_left_over_pieces()
{
    left_over_pieces_.erase(
        std::remove_if(
            left_over_pieces_.begin(),
            left_over_pieces_.end(),
            [](const auto &p) { return p.dimensions_.first <= 0 || p.dimensions_.second <= 0; }),
        left_over_pieces_.end());
}

void Flooring::position_left_over_pieces()
{
    int left_over_col = 0;
    int left_over_row = 1;
    static constexpr auto left_over_offset = 10;
    for (auto &lo : left_over_pieces_)
    {
        lo.position_ = { left_over_col * (plank_size_.first + left_over_offset),
                         room_size_.second + (left_over_row * (plank_size_.second + left_over_offset)) };
        left_over_col++;
        if (left_over_col > 4)
        {
            left_over_col = 0;
            left_over_row++;
        }
    }
}

auto Flooring::find_and_place_usable_piece(
    bool is_sliced_horizontally,
    bool is_sliced_vertically,
    const std::pair<int, int> &size_lookup) -> bool
{
    auto piece = std::find_if(
        begin(left_over_pieces_), end(left_over_pieces_), [&](const auto &p) { return p.dimensions_ >= size_lookup; });
    if (piece == std::end(left_over_pieces_))
    {
        return false;
    }

    if (is_sliced_horizontally)
    {
        (*piece).dimensions_.second -= size_lookup.second;
    }
    if (is_sliced_vertically)
    {
        (*piece).dimensions_.first -= size_lookup.first;
    }

    planks_.emplace_back((*piece).id_, current_position_, size_lookup, (*piece).color_);
    return true;
}

auto Flooring::calculate() -> Result
{
    randomize();

    current_position_ = { 0, 0 };
    stagger_pattern_index_ = 0;

    planks_.clear();
    left_over_pieces_.clear();

    int index = 0;
    int uncut_planks = 0;

    while (current_position_.second < room_size_.second)
    {
        const auto slice = calculate_slice();
        const bool is_sliced_vertically = slice.first > 0;
        const bool is_sliced_horizontally = slice.second > 0;

        if (!is_sliced_horizontally && !is_sliced_vertically)
        {
            index++;
            uncut_planks++;
            planks_.emplace_back(index, current_position_, plank_size_, generate_color());
            increment_position(plank_size_);
            continue;
        }

        // dimensions we are looking for
        const std::pair<int, int> size_lookup{ is_sliced_vertically ? slice.first : plank_size_.first,
                                               is_sliced_horizontally ? slice.second : plank_size_.second };

        if (!find_and_place_usable_piece(is_sliced_horizontally, is_sliced_vertically, size_lookup))
        {
            const auto color = generate_color();

            // make a new plank
            index++;
            planks_.emplace_back(index, current_position_, size_lookup, color);

            // store the remaining part in the left overs
            const std::pair<int, int> left_over_dimensions = {
                is_sliced_vertically ? plank_size_.first - size_lookup.first : plank_size_.first,
                is_sliced_horizontally ? plank_size_.second - size_lookup.second : plank_size_.second
            };
            left_over_pieces_.emplace_back(index, std::pair<int, int>{ 0, 0 }, left_over_dimensions, color);
        }

        cleanup_left_over_pieces();
        increment_position(size_lookup);
    }

    position_left_over_pieces();

    return { index, static_cast<int>(left_over_pieces_.size()), uncut_planks, planks_, left_over_pieces_ };
}
