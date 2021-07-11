#include "Block.hpp"
#include "Color.hpp"
#include "Transforms.hpp"
#include "jsonlevelconfig.hpp"

#include <gtest/gtest.h>

namespace test {

TEST(TestTransforms, transform) {
  using enum color::Color;
  Transforms t;
  t.add_level_type_override('a', json::rotating_colors("bc"));
  t.add_level_type_override('b', json::rotating_colors("cdc"));

  auto [af_block, af_color]       = t.do_transform("a", RuleSide::FROM);
  auto [bf_block, bf_color]       = t.do_transform("b", RuleSide::FROM);
  auto [cf_block, cf_color]       = t.do_transform("c", RuleSide::FROM);
  auto [at_block, at_color]       = t.do_transform("a", RuleSide::TO);
  auto [bt_block, bt_color]       = t.do_transform("b", RuleSide::TO);
  auto [ct_block, ct_color]       = t.do_transform("c", RuleSide::TO);
  auto [wf_block, wf_color]       = t.do_transform(".", RuleSide::FROM);
  auto [wt_block, wt_color]       = t.do_transform(".", RuleSide::TO);
  auto [nothf_block, nothf_color] = t.do_transform("!", RuleSide::FROM);
  auto [notht_block, notht_color] = t.do_transform("!", RuleSide::TO);
  auto [br1f_block, br1f_color]   = t.do_transform("1", RuleSide::FROM);
  auto [br2f_block, br2f_color]   = t.do_transform("2", RuleSide::FROM);
  auto [br3f_block, br3f_color]   = t.do_transform("3", RuleSide::FROM);
  auto [br1t_block, br1t_color]   = t.do_transform("1", RuleSide::TO);
  auto [br2t_block, br2t_color]   = t.do_transform("2", RuleSide::TO);
  auto [br3t_block, br3t_color]   = t.do_transform("3", RuleSide::TO);

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

  EXPECT_EQ(b1, nothf_block);
  EXPECT_EQ(b1, notht_block);
  EXPECT_EQ(RuleSide::FROM, get_rule_side(nothf_color));
  EXPECT_EQ(RuleSide::TO, get_rule_side(notht_color));
  EXPECT_EQ(NOTHING, get_color(nothf_color));
  EXPECT_EQ(NOTHING, get_color(notht_color));

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

TEST(TestTransforms, unfinalize_block) {
  using enum color::Color;
  using enum RuleSide;

  Transforms t;
  t.add_level_type_override('a', json::rotating_colors("bc"));
  t.add_level_type_override('b', json::rotating_colors("cdc"));

  auto b1 = block::FinalBlock{1};
  auto b2 = block::FinalBlock{2};
  auto b3 = block::FinalBlock{3};

  auto noth_fm = to_final_color(NOTHING, FROM); // not actually used
  auto noth_to = to_final_color(NOTHING, TO);

  auto rect_to = to_final_color(SOLID_RECTANGLE, TO);
  auto rect_fm = to_final_color(SOLID_RECTANGLE, FROM);
  auto wild_to = to_final_color(WILDCARD, TO);
  auto wild_fm = to_final_color(WILDCARD, FROM);
  auto bref_to = to_final_color(BACKREF, TO);
  auto bref_fm = to_final_color(BACKREF, FROM);
  auto cust_to = to_final_color(NEXT_CUSTOM, TO);
  auto cust_fm = to_final_color(NEXT_CUSTOM, FROM);

  EXPECT_EQ('!', t.unfinalize_block(b1, noth_fm));
  EXPECT_EQ('!', t.unfinalize_block(b1, noth_to));

  EXPECT_EQ('a', t.unfinalize_block(b1, rect_fm));
  EXPECT_EQ('a', t.unfinalize_block(b1, rect_to));

  EXPECT_EQ('.', t.unfinalize_block(b1, wild_fm));
  EXPECT_EQ('.', t.unfinalize_block(b1, wild_to));

  EXPECT_EQ('a', t.unfinalize_block(b1, cust_fm));
  EXPECT_EQ('a', t.unfinalize_block(b1, cust_to));
  EXPECT_EQ('b', t.unfinalize_block(b2, cust_fm));
  EXPECT_EQ('b', t.unfinalize_block(b2, cust_to));

  EXPECT_EQ('1', t.unfinalize_block(b1, bref_fm));
  EXPECT_EQ('1', t.unfinalize_block(b1, bref_to));
  EXPECT_EQ('2', t.unfinalize_block(b2, bref_fm));
  EXPECT_EQ('2', t.unfinalize_block(b2, bref_to));
  EXPECT_EQ('3', t.unfinalize_block(b3, bref_fm));
  EXPECT_EQ('3', t.unfinalize_block(b3, bref_to));
}

} // namespace test
