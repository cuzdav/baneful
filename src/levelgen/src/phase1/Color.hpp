#pragma once

#include "RuleSide.hpp"
#include <cstdint>

namespace p1::color {

  // NOTE: Color is not red, blue, green, etc. It is a graph theory annotation
  // to the type of vertex in our rule graph. More thought of as "block type",
  // But not all colors are known at compile time, as some are configured at
  // runtime. Those are NEXT_CUSTOM + (offset), to make unique colors. The
  // lowest bit is a secondary attribute of the color, for whether it appears on
  // the "FROM" pattern (1) or on the right-hand side, the "TO" pattern (0)

  enum class Color : std::uint8_t {
    TO              = 0, // bit
    FROM            = 1, // bit (OR'd with rest of color value)

    SOLID_RECTANGLE = 2,
    DEFAULT         = SOLID_RECTANGLE,
    WILDCARD        = 3,
    BACKREF         = 4,

    NEXT_CUSTOM,
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
