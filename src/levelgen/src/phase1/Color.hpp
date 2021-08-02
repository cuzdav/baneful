#pragma once

#include "RuleSide.hpp"
#include "enum_utils.hpp"
#include <cassert>
#include <cstdint>
#include <string>
#include <string_view>

namespace color {

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
  UNUSED_         = 0,
  NOTHING         = 1, // "Goes to nothing i.e., "a" -> ""
  SOLID_RECTANGLE = 2,
  DEFAULT         = SOLID_RECTANGLE,
  WILDCARD        = 3,
  BACKREF         = 4,
  ROTATING_COLORS = 5,

  NEXT_CUSTOM,
};

constexpr bool
is_mergeable(Color color) {
  using enum Color;
  switch (color) {
  case SOLID_RECTANGLE:
  case WILDCARD:
  case BACKREF: return true;
  }
  return false;
}

constexpr char
color_as_char(Color color) {
  using enum Color;
  switch (color) {
  case UNUSED_: return ' ';
  case NOTHING: return '!';
  case SOLID_RECTANGLE: return 'R';
  case WILDCARD: return '.';
  case BACKREF: return 'B';
  case ROTATING_COLORS: return 'T'; // roTating
  default: return 'U' - +NEXT_CUSTOM + +color;
  }
}

constexpr Color
char_as_color(char c) {
  using enum Color;
  Color color;
  switch (c) {
  case 'R': color = SOLID_RECTANGLE; break;
  case 'B': color = BACKREF; break;
  case '.': color = WILDCARD; break;
  case '!': color = NOTHING;
  case 'T': color = ROTATING_COLORS;
  default: color = Color(+c - 'U' + +NEXT_CUSTOM);
  }
  return color;
}

inline std::string
to_string(Color color) {
  using enum Color;
  switch (color) {

  case UNUSED_: return "[UNUSED!]";
  case NOTHING: return "NOTHING";
  case SOLID_RECTANGLE: return "SOLID_RECTANGLE";
  case WILDCARD: return "WILDCARD";
  case BACKREF: return "BACKREF";
  case ROTATING_COLORS: return "ROTATING_COLORS";
  default: return "CUSTOM+" + std::to_string(+color - +NEXT_CUSTOM);
  }
}

// Combines pure Color and a RuleSide enum
enum class FinalColor : std::uint8_t {};

constexpr FinalColor
to_final_color(Color color, RuleSide side) {
  return FinalColor((+color << 1) | +side);
}

// remove FROM bit from color (from/to is in low bit)
constexpr Color
get_color(FinalColor color) {
  return Color(+color >> 1);
}

constexpr RuleSide
get_rule_side(FinalColor color) {
  return RuleSide(+color & 1);
}

constexpr bool
has_from(FinalColor color) {
  return get_rule_side(color) == RuleSide::FROM;
}

inline std::string
to_long_string(FinalColor final_color) {
  return "FinalColor(" + std::to_string(+final_color) +
         "):" + to_string(get_color(final_color)) + ":" +
         to_string(get_rule_side(final_color));
}

inline std::string
to_string(FinalColor final_color) {
  return to_string(get_rule_side(final_color)) + ':' +
         to_string(get_color(final_color));
}

inline std::string
to_short_string(FinalColor fcolor) {
  return std::string(has_from(fcolor) ? "F" : "T") +
         color_as_char(get_color(fcolor));
}

// internal printable-char representation of colors
inline FinalColor
short_string_to_color(std::string_view short_str) {
  assert(short_str.size() == 2);
  RuleSide side  = short_str[0] == 'F' ? RuleSide::FROM : RuleSide::TO;
  Color    color = char_as_color(short_str[1]);
  return to_final_color(color, side);
}

} // namespace color
