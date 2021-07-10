#pragma once

#include "Block.hpp"
#include "Color.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>

namespace p1::vertex {

// A vertex is a minimal data structor layered inside a uint32_t.
// It may hold 1 or more blocks (up to 6... AAAA-FFFF)
// YYYY is the main color, while X is the From/To indicator
// [???][YYYYX][AAAA][BBBB][CCCC][DDDD][EEEE][FFFF]

// Blocks are all in terms of FinalBlock. This is a 0-based number assigned to
// the current block, for its given color.  See Transforms.

// bit shifting for internal fields
static constexpr std::uint32_t ColorShift         = 24;
static constexpr std::uint32_t Block1Shift        = 20;
static constexpr std::uint32_t Block2Shift        = 16;
static constexpr std::uint32_t Block3Shift        = 12;
static constexpr std::uint32_t Block4Shift        = 8;
static constexpr std::uint32_t Block5Shift        = 4;
static constexpr std::uint32_t Block6Shift        = 0;
static constexpr std::uint32_t MaxBlocksPerVertex = 6;
static constexpr std::uint32_t BitsPerBlock       = 4;
static constexpr std::uint32_t BlockMask          = (1 << BitsPerBlock) - 1;
static constexpr std::uint32_t BitsForColor       = 5;
static constexpr std::uint32_t ColorMask          = (1 << BitsForColor) - 1;

enum class Vertex : std::uint32_t {};

std::string to_string(Vertex vertex);

constexpr Vertex
add_block(Vertex vertex, block::FinalBlock block) {
  auto v     = +vertex;
  auto shift = ((v >> Block1Shift) & BlockMask) == 0 ? Block1Shift
             : ((v >> Block2Shift) & BlockMask) == 0 ? Block2Shift
             : ((v >> Block3Shift) & BlockMask) == 0 ? Block3Shift
             : ((v >> Block4Shift) & BlockMask) == 0 ? Block4Shift
             : ((v >> Block5Shift) & BlockMask) == 0 ? Block5Shift
             : ((v >> Block6Shift) & BlockMask) == 0
                 ? Block6Shift
                 : throw std::runtime_error("Vertex is Full");

  return Vertex{v | (std::uint32_t(+block) << shift)};
}

// "final_color" is normal color with the the From/To bit already set (or
// unset) properly (see Color.h to finalize color)
// "final_block" will be the first block added to this vertex.
constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block) {
  std::uint32_t value = +final_color << ColorShift;
  Vertex        vertex{value};
  return add_block(vertex, block);
}

constexpr color::FinalColor
get_final_color(Vertex vertex) {
  return color::FinalColor{std::uint8_t((+vertex >> ColorShift) & ColorMask)};
}

constexpr Vertex
set_final_color(Vertex vertex, color::FinalColor color) {
  return Vertex{(+vertex & (~ColorMask << ColorShift)) |
                (+color << ColorShift)};
}

constexpr block::FinalBlock
get_block(Vertex vertex, std::uint8_t idx) {
  auto bits = +vertex >> (Block1Shift - idx * BitsPerBlock);
  return block::FinalBlock{std::uint8_t(bits & BlockMask)};
}

constexpr std::array<block::FinalBlock, MaxBlocksPerVertex>
get_blocks(Vertex vertex) {
  return {
      block::FinalBlock((+vertex >> Block1Shift) & BlockMask),
      block::FinalBlock((+vertex >> Block2Shift) & BlockMask),
      block::FinalBlock((+vertex >> Block3Shift) & BlockMask),
      block::FinalBlock((+vertex >> Block4Shift) & BlockMask),
      block::FinalBlock((+vertex >> Block5Shift) & BlockMask),
      block::FinalBlock((+vertex >> Block6Shift) & BlockMask),
  };
}

constexpr bool
same_color(Vertex a, Vertex b) {
  return get_final_color(a) == get_final_color(b);
}

constexpr bool
is_full(Vertex vertex) {
  // the "last" block is in the lowest bits.
  return +vertex & BlockMask;
}

constexpr int
available_spaces(Vertex vertex) {
  return (((+vertex >> Block1Shift) & BlockMask) == 0) +
         (((+vertex >> Block2Shift) & BlockMask) == 0) +
         (((+vertex >> Block3Shift) & BlockMask) == 0) +
         (((+vertex >> Block4Shift) & BlockMask) == 0) +
         (((+vertex >> Block5Shift) & BlockMask) == 0) +
         (((+vertex >> Block6Shift) & BlockMask) == 0);
}

constexpr int
num_blocks(Vertex vertex) {
  return MaxBlocksPerVertex - available_spaces(vertex);
}

constexpr bool
can_merge(Vertex a, Vertex b) {
  return same_color(a, b) && available_spaces(a) <= num_blocks(b);
}

constexpr Vertex
merge(Vertex a, Vertex b) {
  return {};
}

} // namespace p1::vertex
