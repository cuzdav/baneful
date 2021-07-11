#include "Block.hpp"
#include "enumutils.hpp"
#include "Color.hpp"
#include "Transforms.hpp"
#include "Vertex.hpp"
#include <cassert>
#include <stdexcept>
#include <utility>

namespace vertex {

std::string
to_string(Vertex vertex) {
  std::string  blocks;
  char const * sep = "";
  for (block::FinalBlock block : get_blocks(vertex)) {
    if (+block == 0) {
      break;
    }
    blocks += std::exchange(sep, ", ");
    blocks += std::to_string(+block);
  }

  return "#<Vertex:" + to_string(get_final_color(vertex)) + ":" + blocks + ">";
}

std::string
to_external_name(Vertex v, Transforms const & transforms) {
  std::string       result = "[";
  color::FinalColor color  = get_final_color(v);
  result += color_to_char(color);
  result += ':';
  for (block::FinalBlock block : get_blocks(v)) {
    result += transforms.unfinalize_block(block, color);
  }
  return result + ']';
}

} // namespace vertex
