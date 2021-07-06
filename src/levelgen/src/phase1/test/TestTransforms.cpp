#include "Block.hpp"
#include "Color.hpp"
#include "Vertex.hpp"
#include "Verticies.hpp"
#include "Transforms.hpp"
#include "jsonlevelconfig.hpp"

#include <gtest/gtest.h>
#include <string_view>
#include <iostream>

using namespace boost;
using namespace std::literals;

namespace p1::test {

TEST(TestTransforms, BasicInterface) {
  Transforms t;

  t.add_level_type_override('a', rotating_colors("bc"));

  auto expected_color  = to_final_color(color::Color::NEXT_CUSTOM, RuleSide::TO);
  auto expected_vertex = vertex::create(expected_color, block::FinalBlock{1});
  add_block(expected_vertex, block::FinalBlock{2});
  add_block(expected_vertex, block::FinalBlock{3});

  auto [block, color] = t.do_transform("ab", RuleSide::TO);
  //    EXPECT_EQ(expected);
  std::cout << to_string(block) << ", color= " << to_string(color) << std::endl;
}

} // namespace p1::test
