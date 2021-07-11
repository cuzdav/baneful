#include "Block.hpp"
#include "Color.hpp"
#include "Vertex.hpp"
#include "Vertices.hpp"
#include "Transforms.hpp"
#include "jsonlevelconfig.hpp"

#include <gtest/gtest.h>
#include <string_view>
#include <iostream>

namespace test {

using namespace boost;
using namespace std::literals;

TEST(TestTransforms, transform) {
  using enum color::Color;
  Transforms t;
  t.add_level_type_override('a', rotating_colors("bc"));
  t.add_level_type_override('b', rotating_colors("cdc"));

  auto [af_block, af_color]     = t.do_transform("a", RuleSide::FROM);
  auto [bf_block, bf_color]     = t.do_transform("b", RuleSide::FROM);
  auto [cf_block, cf_color]     = t.do_transform("c", RuleSide::FROM);
  auto [at_block, at_color]     = t.do_transform("a", RuleSide::TO);
  auto [bt_block, bt_color]     = t.do_transform("b", RuleSide::TO);
  auto [ct_block, ct_color]     = t.do_transform("c", RuleSide::TO);
  auto [wf_block, wf_color]     = t.do_transform(".", RuleSide::FROM);
  auto [wt_block, wt_color]     = t.do_transform(".", RuleSide::TO);
  auto [br1f_block, br1f_color] = t.do_transform("1", RuleSide::FROM);
  auto [br2f_block, br2f_color] = t.do_transform("2", RuleSide::FROM);
  auto [br3f_block, br3f_color] = t.do_transform("3", RuleSide::FROM);
  auto [br1t_block, br1t_color] = t.do_transform("1", RuleSide::TO);
  auto [br2t_block, br2t_color] = t.do_transform("2", RuleSide::TO);
  auto [br3t_block, br3t_color] = t.do_transform("3", RuleSide::TO);

  auto b1 = block::FinalBlock{1};
  auto b2 = block::FinalBlock{2};
  auto b3 = block::FinalBlock{3};

  EXPECT_EQ(b1, af_block);
  EXPECT_EQ(b1, at_block);
  EXPECT_EQ(RuleSide::FROM, get_rule_side(af_color));
  EXPECT_EQ(RuleSide::TO, get_rule_side(at_color));
  EXPECT_EQ(NEXT_CUSTOM, get_color(af_color));
  EXPECT_EQ(NEXT_CUSTOM, get_color(at_color));

  EXPECT_EQ(b2, bf_block);
  EXPECT_EQ(b2, bt_block);
  EXPECT_EQ(RuleSide::FROM, get_rule_side(bf_color));
  EXPECT_EQ(RuleSide::TO, get_rule_side(bt_color));
  EXPECT_EQ(+NEXT_CUSTOM + 1, +get_color(bf_color));
  EXPECT_EQ(+NEXT_CUSTOM + 1, +get_color(bt_color));

  EXPECT_EQ(b3, cf_block);
  EXPECT_EQ(b3, ct_block);
  EXPECT_EQ(RuleSide::FROM, get_rule_side(cf_color));
  EXPECT_EQ(RuleSide::TO, get_rule_side(ct_color));
  EXPECT_EQ(SOLID_RECTANGLE, get_color(cf_color));
  EXPECT_EQ(SOLID_RECTANGLE, get_color(ct_color));

  EXPECT_EQ(b1, wf_block);
  EXPECT_EQ(b1, wt_block);
  EXPECT_EQ(RuleSide::FROM, get_rule_side(wf_color));
  EXPECT_EQ(RuleSide::TO, get_rule_side(wt_color));
  EXPECT_EQ(WILDCARD, get_color(wf_color));
  EXPECT_EQ(WILDCARD, get_color(wt_color));

  EXPECT_EQ(b1, br1f_block);
  EXPECT_EQ(b1, br1t_block);
  EXPECT_EQ(RuleSide::FROM, get_rule_side(br1f_color));
  EXPECT_EQ(RuleSide::TO, get_rule_side(br1t_color));
  EXPECT_EQ(BACKREF, get_color(br1f_color));
  EXPECT_EQ(BACKREF, get_color(br1t_color));

  EXPECT_EQ(b2, br2f_block);
  EXPECT_EQ(b2, br2t_block);
  EXPECT_EQ(RuleSide::FROM, get_rule_side(br2f_color));
  EXPECT_EQ(RuleSide::TO, get_rule_side(br2t_color));
  EXPECT_EQ(BACKREF, get_color(br2f_color));
  EXPECT_EQ(BACKREF, get_color(br2t_color));
}

} // namespace test
