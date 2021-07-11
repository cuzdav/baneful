#include "Block.hpp"
#include "enumutils.hpp"
#include "Color.hpp"
#include "Transforms.hpp"
#include "Vertex.hpp"
#include <cassert>
#include <stdexcept>
#include <utility>

namespace vertex {

namespace {
template <typename ColorPolicy>
std::string
to_external_name(Vertex v, Transforms const & transforms,
                 ColorPolicy color_printer) {
  std::string       result = "[";
  color::FinalColor color  = get_final_color(v);
  result += color_printer(color);
  result += ':';
  for (block::FinalBlock block : get_blocks(v)) {
    if (+block == 0) {
      break;
    }
    result += transforms.unfinalize_block(block, color);
  }
  return result + ']';
}

} // namespace

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
to_external_short_name(Vertex v, Transforms const & transforms) {
  return to_external_name(v, transforms, color::color_to_char);
}

std::string
to_external_name(Vertex v, Transforms const & transforms) {
  return to_external_name(
      v, transforms,
      static_cast<std::string (*)(color::FinalColor)>(color::to_string));
}

} // namespace vertex
