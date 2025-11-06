#pragma once

#include <type_traits> // For std::is_standard_layout

namespace TuiRogGame {
namespace Domain {
namespace Model {

struct Position {
    int x = 0;
    int y = 0;

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }

    bool operator<(const Position& other) const {
        if (x != other.x) {
            return x < other.x;
        }
        return y < other.y;
    }};

// Verify at compile time that this struct has a standard layout, which is crucial for serialization.
static_assert(std::is_standard_layout<Position>::value, "Position must be a standard layout type for serialization.");

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame