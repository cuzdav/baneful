#pragma once

#include "Block.hpp"
#include "Color.hpp"

#include <array>
#include <cassert>
#include <cstdint>


namespace p1::vertex {

  static constexpr std::uint32_t ColorShift = 24;
  static constexpr std::uint32_t Block1Shift = 20;
  static constexpr std::uint32_t Block2Shift = 16;
  static constexpr std::uint32_t Block3Shift = 12;
  static constexpr std::uint32_t Block4Shift = 8;
  static constexpr std::uint32_t Block5Shift = 4;
  static constexpr std::uint32_t Block6Shift = 0;
  static constexpr std::uint32_t MaxBlocksPerVertex = 6;
  static constexpr std::uint32_t BitsPerBlock = 4;
  static constexpr std::uint32_t BlockMask = (1<<BitsPerBlock)-1;
  static constexpr std::uint32_t BitsForColor = 5;
  static constexpr std::uint32_t ColorMask = (1<<BitsForColor) - 1;

  // A vertex may hold 1 or more blocks (up to 6... AAAA-FFFF)
  // YYYY is the main color, while X is the From/To indicator
  // [???][YYYYX][AAAA][BBBB][CCCC][DDDD][EEEE][FFFF]
  enum class Vertex : std::uint32_t{};

  inline constexpr std::uint32_t
  operator+(Vertex v) { return static_cast<std::uint32_t>(v); }

  // "final_color" is normal color with the the From/To bit already set (or
  // unset) properly
  // "final_block" is the first block, already transformed.
  Vertex
  create(color::Color final_color, block::FinalBlock block);

  Vertex
  add_block(Vertex vertex, block::FinalBlock block);

  color::Color
  get_color(Vertex vertex);

  block::FinalBlock
  get_block(Vertex vertex, std::uint8_t idx);

  std::array<block::FinalBlock, MaxBlocksPerVertex>
  get_blocks(Vertex vertex);

  std::string
  to_string(Vertex vertex);

} // p1
