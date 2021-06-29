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
  get_color(Vertex v) {
    return color::Color{
      std::uint8_t((+v >> ColorShift) & ColorMask)
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

    return Vertex{ +v | (std::uint32_t(+block) << shift) };
  }

  block::FinalBlock
  get_block(Vertex v, std::uint8_t idx) {
    auto bits = +v >> (Block1Shift - idx * BitsPerBlock);

    return block::FinalBlock{std::uint8_t(bits & BlockMask)};
  }

  std::array<block::FinalBlock, MaxBlocksPerVertex>
  get_blocks(Vertex v)
  {
    return {
      block::FinalBlock((+v >> Block1Shift) & BlockMask),
      block::FinalBlock((+v >> Block2Shift) & BlockMask),
      block::FinalBlock((+v >> Block3Shift) & BlockMask),
      block::FinalBlock((+v >> Block4Shift) & BlockMask),
      block::FinalBlock((+v >> Block5Shift) & BlockMask),
      block::FinalBlock((+v >> Block6Shift) & BlockMask),
    };
  }
}
