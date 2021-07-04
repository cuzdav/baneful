#pragma once

#include "RuleSide.hpp"
#include <cstdint>
#include <string>

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
  inline constexpr std::uint32_t operator+(Color c) {
    return static_cast<std::uint32_t>(c);
  }

  inline constexpr Color operator|(Color a, Color b) {
    return Color(+a | +b);
  }

  inline constexpr Color for_side(Color color, RuleSide side) {
    return color | (RuleSide::FROM == side ? Color::FROM : Color::TO);
  }

  // remove FROM bit from color
  inline constexpr Color pure(Color color) {
    return Color(+color & ~+Color::FROM);
  }

  inline constexpr bool has_from(Color color) {
    return color == pure(color);
  }

  inline std::string to_string_pure(Color color) {
    switch (pure(color)) {
    case Color::TO:              return "TO";
    case Color::FROM:            return "FROM";
    case Color::SOLID_RECTANGLE: return "SOLID_RECTANGLE";
    case Color::WILDCARD:        return "WILDCARD";
    case Color::BACKREF:         return "BACKREF";
    default:                     return "CUSTOM";
    }
  }

  inline std::string to_string(Color color) {
    auto side = has_from(color) ? ":FROM:" : ":TO:";
    auto value = std::to_string(std::uint8_t(+color));
    return "#<Color:" + value + side + to_string_pure(color) + ">";
  }

} // p1
