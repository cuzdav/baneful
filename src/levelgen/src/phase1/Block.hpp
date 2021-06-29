#pragma once

#include <cstdint>

namespace p1::block {

  // just a char from a rule json, as in ('a' -> "bc")
  // but the type indicates that it has been fixed up in its
  // final form by the transform, which makes it a 0-based value
  enum FinalBlock : std::uint8_t {};

  inline constexpr FinalBlock to_block(char c) {
    return FinalBlock(std::uint8_t(c));
  };

  inline constexpr std::uint8_t
  operator+(FinalBlock b) {
    return static_cast<std::uint8_t>(b);
  }

}
