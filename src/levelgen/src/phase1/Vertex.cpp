#include "enumutils.hpp"
#include "Vertex.hpp"
#include <cassert>
#include <stdexcept>
#include <utility>

namespace p1::vertex {

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

} // namespace p1::vertex
