#include <string_view>
#include <gtest/gtest.h>

#include "Color.hpp"
#include "RuleSide.hpp"
#include "Vertices.hpp"

using namespace p1;
using namespace std::literals;

static constexpr block::FinalBlock empty_block = block::FinalBlock{'\0'};

static constexpr color::FinalColor fc_rect_from =
    to_final_color(p1::color::Color::SOLID_RECTANGLE, p1::RuleSide::FROM);
static constexpr color::FinalColor fc_rect_to =
    to_final_color(p1::color::Color::SOLID_RECTANGLE, p1::RuleSide::TO);

static constexpr color::FinalColor fc_wc_from =
    to_final_color(p1::color::Color::WILDCARD, p1::RuleSide::FROM);

TEST(TestVertices, BasicInterface) {
  Vertices v;
  EXPECT_EQ(v.names_end(), v.names_begin());
  EXPECT_EQ(0, v.names_size());

  Vertices const v2;
  EXPECT_EQ(v2.names_end(), v2.names_begin());
  EXPECT_EQ(0, v2.names_size());
}

TEST(TestVertices, AddVertexSingleDefault) {
  Vertices v;

  int idx = v.add_vertex_single("abc"sv, empty_block, fc_rect_from);
  EXPECT_EQ(1, v.names_size());

  auto abc_internal = Vertices::internal_name("abc", fc_rect_from);
  EXPECT_EQ(abc_internal, v.name_of(idx));

  int idx2 = v.add_vertex_single("abc"sv, empty_block, fc_rect_from);
  EXPECT_EQ(1, v.names_size());
  EXPECT_EQ(idx, idx2);
}

TEST(TestVertices, AddVertexSingle_SameNameDifferentColor) {
  Vertices v;

  int idx  = v.add_vertex_single("abc"sv, empty_block, fc_rect_from);
  int idx2 = v.add_vertex_single("abc"sv, empty_block, fc_wc_from);
  EXPECT_EQ(2, v.names_size());
  EXPECT_EQ(0, idx);
  EXPECT_EQ(1, idx2);

  auto iter = v.names_begin();
  ASSERT_NE(v.names_end(), iter);
  EXPECT_EQ(v.name_of(0), *iter);
  ++iter;
  ASSERT_NE(v.names_end(), iter);
  EXPECT_EQ(v.name_of(1), *iter);
  ++iter;
  ASSERT_EQ(v.names_end(), iter);
}

TEST(TestVertices, IndexOfs) {
  Vertices v;

  // three-node chain "a->b->c"
  int idx1 = v.add_vertex_single("abc"sv, empty_block, fc_rect_from);
  int idx2 = v.add_vertex_single("bc"sv, empty_block, fc_rect_from);
  int idx3 = v.add_vertex_single("c"sv, empty_block, fc_rect_from);

  int actual1 = v.name_index_of("abc", fc_rect_from);
  int actual2 = v.name_index_of("bc", fc_rect_from);
  int actual3 = v.name_index_of("c", fc_rect_from);

  EXPECT_EQ(-1, v.name_index_of("abc", fc_wc_from));
  EXPECT_EQ(idx1, actual1);
  EXPECT_EQ(idx2, actual2);
  EXPECT_EQ(idx3, actual3);
}
