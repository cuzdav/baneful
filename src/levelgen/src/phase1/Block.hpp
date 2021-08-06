#pragma once

#include "enum_utils.hpp"
#include <cstdint>
#include <string>

namespace block {

// just a char from a rule json, as in ('a' -> "bc")
// but the type indicates that it has been fixed up in its
// final form by the transform, which makes it a 0-based value
// (where 0 indicates an unset/unused value)
enum FinalBlock : std::uint8_t {};

static const FinalBlock Unused{0};

// In json and elsewhere, nothing blocks are the empty string, but in our
// vertex code, it needs a char at least to be added to a vertex. It's
// arbitrary, but ! is the synthetic assigned char when we see an empty string
// meaning NOTHING.
static constexpr char       NOTHING_BLOCK_CHAR = '!';
static constexpr char const NOTHING_BLOCK_CSTR[2]{NOTHING_BLOCK_CHAR};

inline constexpr FinalBlock
to_block(char c) {
  return FinalBlock(std::uint8_t(c));
};

inline std::string
to_string(FinalBlock block) {
  return "Block:(" + std::to_string(+block) + ")";
}

} // namespace block
