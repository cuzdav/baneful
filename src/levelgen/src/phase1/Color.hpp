#pragma once

#include "RuleSide.hpp"
#include "enumutils.hpp"
#include <cstdint>
#include <string>

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
  UNUSED_ = 0,
  NOTHING = 1, // "Goes to nothing i.e., "a" -> ""
  SOLID_RECTANGLE,
  DEFAULT     = SOLID_RECTANGLE,
  WILDCARD    = 3,
  BACKREF     = 4,
  NEXT_CUSTOM = 5,
};

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
to_string(Color color) {
  using enum Color;
  switch (color) {

  case UNUSED_: return "[UNUSED!]";
  case NOTHING: return "NOTHING";
  case SOLID_RECTANGLE: return "SOLID_RECTANGLE";
  case WILDCARD: return "WILDCARD";
  case BACKREF: return "BACKREF";
  default: return "CUSTOM+" + std::to_string(+color - +NEXT_CUSTOM);
  }
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

// An arbitrary offset past ' ' so that value is shifted into printable range.
// Start at " makes computed values be after it, so '"' isn't a name, which is
// ugly to use as a marker char.
constexpr char CHAR_OFFSET = '\"';

// internal printable-char representation of colors
constexpr char
color_to_char(FinalColor fcolor) {
  return char(+fcolor + CHAR_OFFSET);
}

// internal printable-char representation of colors
constexpr FinalColor
char_to_color(char color_char) {
  return color::FinalColor(color_char - CHAR_OFFSET);
}

} // namespace color
