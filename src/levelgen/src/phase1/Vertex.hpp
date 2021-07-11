#pragma once

#include "Block.hpp"
#include "Color.hpp"
#include "VertexBitConstants.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>

namespace vertex {

// A vertex is a minimal data structor layered inside a uint32_t.
// It may hold 1 or more blocks (up to 6... AAAA-FFFF)
// YYYY is the main color, while X is the From/To indicator
// [???][YYYYX][AAAA][BBBB][CCCC][DDDD][EEEE][FFFF]

// Blocks are all in terms of FinalBlock. This is a 0-based number assigned to
// the current block, for its given color.  See Transforms.

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

constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block1,
       block::FinalBlock block2) {
  return add_block(create(final_color, block1), block2);
}

constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block1,
       block::FinalBlock block2, block::FinalBlock block3) {
  return add_block(create(final_color, block1, block2), block3);
}

constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block1,
       block::FinalBlock block2, block::FinalBlock block3,
       block::FinalBlock block4) {
  return add_block(create(final_color, block1, block2, block3), block4);
}

constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block1,
       block::FinalBlock block2, block::FinalBlock block3,
       block::FinalBlock block4, block::FinalBlock block5) {
  return add_block(create(final_color, block1, block2, block3, block4), block5);
}

constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block1,
       block::FinalBlock block2, block::FinalBlock block3,
       block::FinalBlock block4, block::FinalBlock block5,
       block::FinalBlock block6) {
  return add_block(create(final_color, block1, block2, block3, block4, block5),
                   block6);
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
size(Vertex vertex) {
  return MaxBlocksPerVertex - available_spaces(vertex);
}

constexpr int
num_can_merge(Vertex a, Vertex b) {
  return std::min(available_spaces(a), size(b));
}

constexpr Vertex
pop_front(Vertex vertex, int num = 1) {
  auto highbits_color = +vertex & ~AllBlocksMask;
  return Vertex{
      (((+vertex & AllBlocksMask) << BitsPerBlock * num) & AllBlocksMask) |
      highbits_color};
}

// Take as many front blocks from b that fit into the available blocks of a.
constexpr Vertex
create_merged(Vertex a, Vertex b) {
  auto b_blockbits  = +b & AllBlocksMask; // remove color bits
  auto shift_blocks = MaxBlocksPerVertex - available_spaces(a);
  return Vertex{+a | (b_blockbits >> BitsPerBlock * shift_blocks)};
}

} // namespace vertex
