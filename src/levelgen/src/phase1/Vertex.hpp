#pragma once

#include "Block.hpp"
#include "Color.hpp"
#include "VertexBitConstants.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>

class Transforms;

namespace vertex {

// A vertex is a minimal data structor layered inside a uint32_t.
// It may hold 1 or more blocks (up to 6... AAAA-FFFF)
// YYYY is the main color, while X is the From/To indicator
// [??][S][YYYYX][AAAA][BBBB][CCCC][DDDD][EEEE][FFFF]

// Blocks are all in terms of FinalBlock. This is a 0-based number assigned to
// the current block, for its given color.  See Transforms.  Any 4-bit block of
// 0 is empty/unset.

// The leading ?? are two unused bits. [S] is 1 bit, set if this vertex
// represents the "start" of a chain. Since nodes fold into each other if they
// are the same, there is one point of ambiguity: a->b and aa->b all use the
// same common vertices: a -> a -> b, as the first one is the "tail" of the
// second, they are not separate. However, what is missing, is knowing if we can
// start with the "inner" 'a' or not. So the first vertex in a chain will have
// the [S] flag set. That would mean a rule of ONLY "aa->b" is clearly different
// from "aa->b, a->b" as in one, only the first 'a' has the start bit set, while
// in the other, both 'a's do.

enum class Vertex : std::uint32_t {};

// In a graph, this designates if this vertex is an entry point, the start of a
// chain or internal (reachable only through other vertices)
enum class VertexRole : bool { INTERNAL, START };

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

constexpr Vertex
set_start_bit(Vertex vertex) {
  return Vertex{+vertex | (1 << StartBitShift)};
}

constexpr bool
get_start_bit(Vertex vertex) {
  return (+vertex >> StartBitShift) & 1;
}

// "final_color" is normal color with the the From/To bit already set (or
// unset) properly (see Color.h to finalize color)
// "final_block" will be the first block added to this vertex.
constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block,
       VertexRole role) {
  std::uint32_t value = +final_color << ColorShift;
  Vertex        vertex{value};
  if (role == VertexRole::START) {
    vertex = set_start_bit(vertex);
  }
  return add_block(vertex, block);
}

constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block1,
       block::FinalBlock block2, VertexRole role) {
  return add_block(create(final_color, block1, role), block2);
}

constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block1,
       block::FinalBlock block2, block::FinalBlock block3, VertexRole role) {
  return add_block(create(final_color, block1, block2, role), block3);
}

constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block1,
       block::FinalBlock block2, block::FinalBlock block3,
       block::FinalBlock block4, VertexRole role) {
  return add_block(create(final_color, block1, block2, block3, role), block4);
}

constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block1,
       block::FinalBlock block2, block::FinalBlock block3,
       block::FinalBlock block4, block::FinalBlock block5, VertexRole role) {
  return add_block(create(final_color, block1, block2, block3, block4, role),
                   block5);
}

constexpr Vertex
create(color::FinalColor final_color, block::FinalBlock block1,
       block::FinalBlock block2, block::FinalBlock block3,
       block::FinalBlock block4, block::FinalBlock block5,
       block::FinalBlock block6, VertexRole role) {
  return add_block(
      create(final_color, block1, block2, block3, block4, block5, role),
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
vertices_are_mergeable(Vertex a, Vertex b) {
  return is_mergeable(get_final_color(a), get_final_color(b)) &&
         get_start_bit(b) == false;
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
  // cannot merge a start bit vertex into another vertex, since the start bit
  // applies to the block at the front--after merging, the start info would not
  // be maintained.
  if (get_start_bit(b)) {
    return 0;
  }
  assert(vertices_are_mergeable(a, b));
  return std::min(available_spaces(a), size(b));
}

constexpr Vertex
pop_front(Vertex vertex, int num = 1) {
  // as an abstract operation it's ok; but in context, we shouldn't pop from a
  // start vertex because that is only done during merging, and merging a start
  // into another vertex either loses the start bit or migrates it to the wrong
  // block at the front of the new vertex.
  assert(get_start_bit(vertex) == false);

  auto highbits = +vertex & ~AllBlocksMask;
  return Vertex{
      (((+vertex & AllBlocksMask) << BitsPerBlock * num) & AllBlocksMask) |
      highbits};
}

// Take as many front blocks from b that fit into the available blocks of a.
constexpr Vertex
create_merged(Vertex a, Vertex b) {
  assert(get_start_bit(b) == false); // see comment in pop_front

  auto b_blockbits  = +b & AllBlocksMask; // remove color bits
  auto shift_blocks = MaxBlocksPerVertex - available_spaces(a);
  return Vertex{+a | (b_blockbits >> BitsPerBlock * shift_blocks)};
}

std::string to_external_name(Vertex v, Transforms const & transforms);
std::string to_external_short_name(Vertex v, Transforms const & transforms);

} // namespace vertex
