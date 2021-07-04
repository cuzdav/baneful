#include "Vertex.hpp"
#include <stdexcept>

namespace p1::vertex {

  // "final_color" is normal color with the the From/To bit already set (or
  // unset) properly
  // "block1" is the already-transformed first block
  Vertex
  create(color::Color final_color, block::FinalBlock block1) {
    auto vertex = Vertex{+final_color << ColorShift};
    return add_block(vertex, block1);
  }

  color::Color
  get_color(Vertex vertex) {
    return color::Color{
      std::uint8_t((+vertex >> ColorShift) & ColorMask)
    };
  }

  Vertex
  add_block(Vertex vertex, block::FinalBlock block) {
    assert((+block & ~BlockMask) == 0);
    auto v = +vertex;

    auto shift =
      ((v >> Block1Shift) & BlockMask) == 0 ? Block1Shift :
      ((v >> Block2Shift) & BlockMask) == 0 ? Block2Shift :
      ((v >> Block3Shift) & BlockMask) == 0 ? Block3Shift :
      ((v >> Block4Shift) & BlockMask) == 0 ? Block4Shift :
      ((v >> Block5Shift) & BlockMask) == 0 ? Block5Shift :
      ((v >> Block6Shift) & BlockMask) == 0 ? Block6Shift :
      throw std::runtime_error("Vertex is Full");

    return Vertex{ v | (std::uint32_t(+block) << shift) };
  }

  block::FinalBlock
  get_block(Vertex vertex, std::uint8_t idx) {
    auto bits = +vertex >> (Block1Shift - idx * BitsPerBlock);

    return block::FinalBlock{std::uint8_t(bits & BlockMask)};
  }

  std::array<block::FinalBlock, MaxBlocksPerVertex>
  get_blocks(Vertex vertex)
  {
    return {
      block::FinalBlock((+vertex >> Block1Shift) & BlockMask),
      block::FinalBlock((+vertex >> Block2Shift) & BlockMask),
      block::FinalBlock((+vertex >> Block3Shift) & BlockMask),
      block::FinalBlock((+vertex >> Block4Shift) & BlockMask),
      block::FinalBlock((+vertex >> Block5Shift) & BlockMask),
      block::FinalBlock((+vertex >> Block6Shift) & BlockMask),
    };
  }

  std::string
  to_string(Vertex vertex) {
    std::string blocks;
    char const * sep = "";
    for (block::FinalBlock block : get_blocks(vertex)) {
      blocks += std::exchange(sep, ", ");
      blocks += +block;    }

    return "#<Vertex:" + to_string(get_color(vertex)) + ":" + blocks + ">";
  }
}
