#pragma once

#include "enumutils.hpp"
#include "RuleSide.hpp"
#include <cstdint>
#include <string>

namespace p1::color {

  // FinalColor is the combination of Color and RuleSide
  // The 1-bit is the RuleSide, and the rest if the color bits shifted up 1.

  // NOTE: Color is not red, blue, green, etc. It is a graph theory annotation
  // to the type of vertex in our rule graph. More thought of as "block type",
  // But not all colors are known at compile time, as some are configured at
  // runtime. Those are NEXT_CUSTOM + (offset), to make unique colors. The
  // lowest bit is a secondary attribute of the color, for whether it appears on
  // the "FROM" pattern (1) or on the right-hand side, the "TO" pattern (0)

  // Color is a category of type of vertex
  enum class Color : std::uint8_t {
    SOLID_RECTANGLE,
    DEFAULT = SOLID_RECTANGLE,
    WILDCARD = 1,
    BACKREF,
    NEXT_CUSTOM,
  };

  // Combines pure Color and a RuleSide enum
  enum class FinalColor : std::uint8_t {};

  inline constexpr FinalColor to_final_color(Color color, RuleSide side) {
    return FinalColor((+color << 1) | +side);
  }

  // remove FROM bit from color
  inline constexpr Color to_color(FinalColor color) {
    return Color(+color >> 1);
  }

  inline constexpr RuleSide to_rule_side(FinalColor color) {
    return RuleSide(+color & 1);
  }

  inline constexpr bool has_from(FinalColor color) {
    return to_rule_side(color) == RuleSide::FROM;
  }

  inline std::string to_string(Color color) {
    switch (color) {
    case Color::SOLID_RECTANGLE: return "SOLID_RECTANGLE";
    case Color::WILDCARD:        return "WILDCARD";
    case Color::BACKREF:         return "BACKREF";
    default:                     return "CUSTOM";
    }
  }

  inline std::string to_string(FinalColor color) {
    return to_string(to_color(color)) + ":" + to_string(to_rule_side(color));
  }

} // p1
