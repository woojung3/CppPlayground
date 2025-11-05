#pragma once

#include <type_traits> // For std::is_standard_layout

namespace TuiRogGame {
namespace Domain {
namespace Model {

struct MapDimensions {
    int width = 0;
    int height = 0;
};

static_assert(std::is_standard_layout<MapDimensions>::value, "MapDimensions must be a standard layout type for serialization.");

} // namespace Model
} // namespace Domain
} // namespace TuiRogGame