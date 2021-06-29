#pragma once

#include "Color.hpp"
#include "Verticies.hpp"

#include <boost/json.hpp>

#include <cassert>
#include <cstdint>
#include <string_view>
#include <map>

/*
  a Transform takes an abstract node in the original json graph and
  represents it in our actual graph. Its responsibilities:

  1) recognize the "color" (type) of the parts

  2) break separate "colors" apart into separate verticies, but
  group "like" adjacent nodes together.



 */

namespace p1 {

  class Transforms {
  public:
    using Color = color::Color;

    Transforms() {
      init_block_maps();
    }

    void
    add_level_type_override(char block,
                            boost::json::object const & type_config) {
      auto const & type = type_config.at("type").as_string();
      if (type == "RotatingColors") {
        auto cycle_chars = type_config.at("cycle_chars").as_string();
        auto custom_name = "Cycle:" + std::string{cycle_chars};
        auto color = get_or_create_color_by_name(custom_name);
        block_to_color_map_[block] = color;
      }
      else {
        throw std::runtime_error("Unknown type_override: " + std::string(type));
      }
    }

    Color
    get_color(char block, RuleSide side) {
      return color::for_side(block_to_color_map_[block], side);
    }

    block::FinalBlock
    finalize_block(char block) {
      auto transformed = block - block_fixup_map_[block];
      assert((transformed & ~vertex::BlockMask) == 0);
      return block::FinalBlock{std::uint8_t(transformed)};
    };

    int
    add_vertex(std::string_view vertex, RuleSide side, Verticies & verticies) {
      char block = vertex[0];
      block::FinalBlock transformed_block = finalize_block(block);
      return verticies.add_vertex_single(vertex,
                                         transformed_block,
                                         get_color(block, side));
    }

  private:
    void
    init_block_maps() {
      for (auto& e : block_to_color_map_) {
        e = Color::DEFAULT;
      }
      for (auto& e : block_fixup_map_) {
        e = 'a'; // assume letters are the main units
      }

      block_to_color_map_['.'] = color::Color::WILDCARD;
      block_fixup_map_['.'] = '.';

      for (char c = '1'; c <= '9'; ++c) {
        block_to_color_map_[c] = color::Color::BACKREF;
        block_fixup_map_[c] = '1'; // '1' -> 0
      }
    }

    Color
    get_or_create_color_by_name(std::string const & color_name) {
      auto
        names_begin = begin(custom_color_names_),
        names_end = end(custom_color_names_),
        name_iter = std::find(names_begin, names_end, color_name);

      auto offset = std::distance(names_begin, name_iter);
      if (name_iter == names_end) {
        custom_color_names_.push_back(color_name);
      }
      return Color(+Color::NEXT_CUSTOM + offset);
    }

  private:
    // given a block (char) what is its color
    Color block_to_color_map_[256];

    // given a block (char) fixup (adj value to be 0-based by...)
    char block_fixup_map_[256];

    // for configuring runtime colors, like Cycle(abc), Cycle(ac), etc.
    // The name is dynamically generated, unknown until it's seen.
    std::vector<std::string> custom_color_names_;
    Color next_custom_color_ = Color::NEXT_CUSTOM;
  };

} // p1
