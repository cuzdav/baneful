#pragma once

#include "RuleSide.hpp"
#include <cstdint>

namespace p1::color {

  // NOTE: Color is not red, blue, green, etc. It is a graph theory annotation
  // to the type of vertex in our rule graph. More thought of as "block type",
  // But not all colors are known at compile time, as some are configured at
  // runtime. Those are NEXT_CUSTOM + (offset), to make unique colors.
  enum class Color : std::uint8_t {
    SOLID_RECTANGLE = 0,
    DEFAULT         = SOLID_RECTANGLE,
    WILDCARD        = 1,
    BACKREF         = 2,

    NEXT_CUSTOM,

    // FLAG BIT FOR LHS  (a -> b, a is FROM|RECTANGLE and b is RECTANGLE)
    FROM            = 1<<7,
    TO              = 0,
  };

  // to underlying
  inline std::uint8_t operator+(Color c) {
    return static_cast<std::uint8_t>(c);
  }

  inline Color operator|(Color a, Color b) {
    return Color(+a | +b);
  }

  inline Color for_side(Color color, RuleSide side) {
    return color | (RuleSide::FROM == side ? Color::FROM : Color::TO);
  }

} // p1
