#pragma once

#include "Block.hpp"
#include "Color.hpp"
#include "VertexBitConstants.hpp"

#include <cassert>
#include <cstdint>
#include <string_view>
#include <tuple>
#include <map>

#include "boost/json.hpp"

/*
  a Transform takes an abstract node in the original json graph and
  represents it in our actual graph. Its responsibilities:

  1) recognize the "color" (type) of the parts

  2) break separate "colors" apart into separate vertex nodes

 */

class Transforms {
public:
  using Color          = color::Color;
  using ColorNames     = std::vector<std::string>;
  using const_iterator = ColorNames::const_iterator;

  Transforms() {
    init_block_maps();
  }

  void
  add_level_type_override(char block, boost::json::object const & type_config) {
    auto const & type = type_config.at("type").as_string();
    if (type == "RotatingColors") {
      auto cycle_chars           = type_config.at("cycle_chars").as_string();
      auto custom_name           = "RotCol:" + std::string{cycle_chars};
      auto color                 = get_or_create_color_by_name(custom_name);
      block_to_color_map_[block] = color;
    }
    else {
      throw std::runtime_error("Unknown type_override: " + std::string(type));
    }
  }

  color::FinalColor
  get_color(char block, RuleSide side) const {
    return color::to_final_color(block_to_color_map_[block], side);
  }

  // Each color has some set of plain-text chars representing block state. A
  // final block is this char transformed into a 0-based value. For example:
  // Color of "SolidRectangle" uses chars 'a', 'b', etc., and as final blocks,
  // 'a' is 0, 'b' is 1, etc. But for a color of BACKREF, the chars a '1', '2',
  // etc, and for that color, '1' is finalized to value of 0, '2' is 1, etc.
  // This is because we only have 4 bits to encode a block, and so can't leave
  // them as chars.
  block::FinalBlock
  finalize_block(char block) const {
    auto transformed = block - block_fixup_map_[block] + 1;
    assert((transformed & ~vertex::BlockMask) == 0);
    return block::FinalBlock{std::uint8_t(transformed)};
  };

  std::tuple<block::FinalBlock, color::FinalColor>
  do_transform(std::string_view vertex, RuleSide side) const {
    assert(vertex.size() > 0);
    char              block             = vertex[0];
    block::FinalBlock transformed_block = finalize_block(block);
    return std::make_tuple(transformed_block, get_color(block, side));
  }

  const_iterator
  begin() const {
    return custom_color_names_.begin();
  }

  const_iterator
  end() const {
    return custom_color_names_.end();
  }

private:
  void
  init_block_maps() {
    for (auto & e : block_to_color_map_) {
      e = Color::DEFAULT;
    }
    for (auto & e : block_fixup_map_) {
      e = 'a'; // assume letters are the main units
    }

    block_to_color_map_[block::NOTHING_BLOCK_CHAR] = Color::NOTHING;
    block_fixup_map_[block::NOTHING_BLOCK_CHAR]    = ' ';

    block_to_color_map_['.'] = Color::WILDCARD;
    block_fixup_map_['.']    = '.';

    for (char c = '1'; c <= '9'; ++c) {
      block_to_color_map_[c] = Color::BACKREF;
      block_fixup_map_[c]    = '1';
    }
  }

  Color
  get_or_create_color_by_name(std::string const & color_name) {
    auto name_iter = std::find(begin(), end(), color_name);
    auto offset    = std::distance(begin(), name_iter);
    if (name_iter == end()) {
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
  ColorNames custom_color_names_;
  Color      next_custom_color_ = Color::NEXT_CUSTOM;
};
