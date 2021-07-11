#include "gtest/gtest.h"
#include "Color.hpp"
#include "Transforms.hpp"
#include "Vertex.hpp"
#include "Vertices.hpp"

namespace vertex::test {

using enum color::Color;
using enum RuleSide;

using FinalColor = color::FinalColor;

constexpr FinalColor frect_color = to_final_color(SOLID_RECTANGLE, FROM),
                     trect_color = to_final_color(SOLID_RECTANGLE, TO),
                     fwild_color = to_final_color(WILDCARD, FROM),
                     twild_color = to_final_color(WILDCARD, TO),
                     fbref_color = to_final_color(BACKREF, FROM),
                     tbref_color = to_final_color(BACKREF, TO);

constexpr block::FinalBlock empty_block = block::FinalBlock{'\0'};

constexpr block::FinalBlock block1 = block::FinalBlock{1};
constexpr block::FinalBlock block2 = block::FinalBlock{2};
constexpr block::FinalBlock block3 = block::FinalBlock{3};
constexpr block::FinalBlock block4 = block::FinalBlock{4};
constexpr block::FinalBlock block5 = block::FinalBlock{5};
constexpr block::FinalBlock block6 = block::FinalBlock{6};

constexpr Vertex v1           = create(frect_color, block1);
constexpr Vertex v12          = add_block(v1, block2);
constexpr Vertex v123         = add_block(v12, block3);
constexpr Vertex v1234        = add_block(v123, block4);
constexpr Vertex v12345       = add_block(v1234, block5);
constexpr Vertex v123456      = add_block(v12345, block6);
constexpr Vertex empty_vertex = create(frect_color, empty_block);

struct VertexMaker {
  template <typename... BlockT>
  Vertex constexpr
  operator()(BlockT... blocks) const {
    return create(color_, blocks...);
  }
  const color::FinalColor color_;
};

Transforms tr;

TEST(TestVertex, enums) {
  char              c = 'C';
  block::FinalBlock b{std::uint8_t(c)};
  char              result = +b;
  EXPECT_EQ('C', result);
}

TEST(TestVertex, Constants) {
  EXPECT_EQ(4, BitsPerBlock);
  EXPECT_EQ(5, BitsForColor);
  EXPECT_EQ(6, MaxBlocksPerVertex);
  EXPECT_EQ(15, BlockMask);
  EXPECT_EQ(31, ColorMask);
}

TEST(TestVertex, VertexEncodingColor) {
  Vertex v1 = create(frect_color, empty_block);
  EXPECT_EQ(frect_color, get_final_color(v1));
}

TEST(TestVertex, color_char_conversions) {
  using enum color::Color;
  auto final_color = to_final_color(SOLID_RECTANGLE, TO);
  auto iname       = Vertices::internal_name("a", final_color);

  // final color is color<<1 to make room for side. SOLID_RECTANGLE is 2, side
  // is 0, so fc is 4.  CHAR_OFFSET('\"')=34, thus 34 + 4 is 38 ().  Space
  // char added to put value into visible/printable character range
  EXPECT_EQ("&a", iname);
}

TEST(TestVertex, VertexEncodingGetBlockSingle) {
  Vertex v1 = create(frect_color, block1);
  EXPECT_EQ(block1, get_block(v1, 0));
  EXPECT_EQ(empty_block, get_block(v1, 1));

  Vertex v2 = add_block(v1, block2);
  EXPECT_EQ(block1, get_block(v2, 0));
  EXPECT_EQ(block2, get_block(v2, 1));
  EXPECT_EQ(empty_block, get_block(v2, 2));

  Vertex v3 = add_block(v2, block3);
  EXPECT_EQ(block1, get_block(v3, 0));
  EXPECT_EQ(block2, get_block(v3, 1));
  EXPECT_EQ(block3, get_block(v3, 2));
  EXPECT_EQ(empty_block, get_block(v3, 3));

  Vertex v4 = add_block(v3, block4);
  EXPECT_EQ(block1, get_block(v4, 0));
  EXPECT_EQ(block2, get_block(v4, 1));
  EXPECT_EQ(block3, get_block(v4, 2));
  EXPECT_EQ(block4, get_block(v4, 3));
  EXPECT_EQ(empty_block, get_block(v4, 4));

  Vertex v5 = add_block(v4, block5);
  EXPECT_EQ(block1, get_block(v5, 0));
  EXPECT_EQ(block2, get_block(v5, 1));
  EXPECT_EQ(block3, get_block(v5, 2));
  EXPECT_EQ(block4, get_block(v5, 3));
  EXPECT_EQ(block5, get_block(v5, 4));
  EXPECT_EQ(empty_block, get_block(v5, 5));

  Vertex v6 = add_block(v5, block6);
  EXPECT_EQ(block1, get_block(v6, 0));
  EXPECT_EQ(block2, get_block(v6, 1));
  EXPECT_EQ(block3, get_block(v6, 2));
  EXPECT_EQ(block4, get_block(v6, 3));
  EXPECT_EQ(block5, get_block(v6, 4));
  EXPECT_EQ(block6, get_block(v6, 5));
}

TEST(TestVertex, VertexEncodingGetBlocks) {
  Vertex v1 = create(frect_color, block1);

  auto blocks = get_blocks(v1);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(empty_block, blocks[1]);

  Vertex v2 = add_block(v1, block2);
  blocks    = get_blocks(v2);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(empty_block, blocks[2]);

  Vertex v3 = add_block(v2, block3);
  blocks    = get_blocks(v3);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(empty_block, blocks[3]);

  Vertex v4 = add_block(v3, block4);
  blocks    = get_blocks(v4);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(block4, blocks[3]);
  EXPECT_EQ(empty_block, blocks[4]);

  Vertex v5 = add_block(v4, block5);
  blocks    = get_blocks(v5);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(block4, blocks[3]);
  EXPECT_EQ(block5, blocks[4]);
  EXPECT_EQ(empty_block, blocks[5]);

  Vertex v6 = add_block(v5, block6);
  blocks    = get_blocks(v6);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(block4, blocks[3]);
  EXPECT_EQ(block5, blocks[4]);
  EXPECT_EQ(block6, blocks[5]);
}

TEST(TestVertex, same_color) {
  using namespace color;
  using enum Color;

  Vertex v1 = create(frect_color, empty_block),
         v2 = create(trect_color, empty_block),
         v3 = create(fwild_color, empty_block),
         v4 = create(twild_color, empty_block),
         v5 = create(fbref_color, empty_block),
         v6 = create(tbref_color, empty_block);

  EXPECT_TRUE(same_color(v1, v1));
  EXPECT_TRUE(same_color(v2, v2));
  EXPECT_TRUE(same_color(v3, v3));
  EXPECT_TRUE(same_color(v4, v4));
  EXPECT_TRUE(same_color(v5, v5));
  EXPECT_TRUE(same_color(v6, v6));

  EXPECT_FALSE(same_color(v1, v2));
  EXPECT_FALSE(same_color(v1, v3));
  EXPECT_FALSE(same_color(v1, v4));
  EXPECT_FALSE(same_color(v1, v5));
  EXPECT_FALSE(same_color(v1, v6));

  EXPECT_FALSE(same_color(v2, v3));
  EXPECT_FALSE(same_color(v2, v4));
  EXPECT_FALSE(same_color(v2, v5));
  EXPECT_FALSE(same_color(v2, v6));

  EXPECT_FALSE(same_color(v3, v4));
  EXPECT_FALSE(same_color(v3, v5));
  EXPECT_FALSE(same_color(v3, v6));

  EXPECT_FALSE(same_color(v4, v5));
  EXPECT_FALSE(same_color(v4, v6));

  EXPECT_FALSE(same_color(v5, v6));
}

TEST(TestVertex, capacity_funcs) {
  Vertex v = create(frect_color, block1);

  EXPECT_EQ(1, size(v));
  EXPECT_EQ(5, available_spaces(v));
  EXPECT_FALSE(is_full(v));

  v = add_block(v, block1);
  EXPECT_EQ(2, size(v));
  EXPECT_EQ(4, available_spaces(v));
  EXPECT_FALSE(is_full(v));

  v = add_block(v, block1);
  EXPECT_EQ(3, size(v));
  EXPECT_EQ(3, available_spaces(v));
  EXPECT_FALSE(is_full(v));

  v = add_block(v, block1);
  EXPECT_EQ(4, size(v));
  EXPECT_EQ(2, available_spaces(v));
  EXPECT_FALSE(is_full(v));

  v = add_block(v, block1);
  EXPECT_EQ(5, size(v));
  EXPECT_EQ(1, available_spaces(v));
  EXPECT_FALSE(is_full(v));

  v = add_block(v, block1);
  EXPECT_EQ(6, size(v));
  EXPECT_EQ(0, available_spaces(v));
  EXPECT_TRUE(is_full(v));
}

TEST(TestVertex, num_can_merge) {
  block::FinalBlock block = block::FinalBlock{1};
  Vertex            v     = create(frect_color, block1);
  Vertex            v2    = create(frect_color, block1, block1, block1);

  // v has 1
  EXPECT_EQ(1, size(v));
  EXPECT_EQ(3, size(v2));
  EXPECT_EQ(3, num_can_merge(v, v2));

  v = add_block(add_block(v, block1), block1);
  // v has 3
  EXPECT_EQ(3, num_can_merge(v, v2));

  v = add_block(v, block1);
  // v has 4
  EXPECT_EQ(2, num_can_merge(v, v2));

  // v has 5
  v = add_block(v, block1);
  EXPECT_EQ(1, num_can_merge(v, v2));

  // v has 6
  v = add_block(v, block1);
  EXPECT_EQ(0, num_can_merge(v, v2));
}

TEST(TestVertex, pop_front_small) {
  Vertex v = create(frect_color, block1, block2);
  EXPECT_EQ(block1, get_block(v, 0));
  EXPECT_EQ(block2, get_block(v, 1));

  v = pop_front(v);
  EXPECT_EQ(block2, get_block(v, 0));
}

TEST(TestVertex, pop_front) {
  Vertex v =
      create(frect_color, block1, block2, block3, block4, block5, block6);
  EXPECT_EQ(block1, get_block(v, 0));
  EXPECT_EQ(block2, get_block(v, 1));
  EXPECT_EQ(block3, get_block(v, 2));
  EXPECT_EQ(block4, get_block(v, 3));
  EXPECT_EQ(block5, get_block(v, 4));
  EXPECT_EQ(block6, get_block(v, 5));

  v = pop_front(v);
  EXPECT_EQ(block2, get_block(v, 0));
  EXPECT_EQ(block3, get_block(v, 1));
  EXPECT_EQ(block4, get_block(v, 2));
  EXPECT_EQ(block5, get_block(v, 3));
  EXPECT_EQ(block6, get_block(v, 4));
  EXPECT_EQ(empty_block, get_block(v, 5));

  v = pop_front(v);
  EXPECT_EQ(block3, get_block(v, 0));
  EXPECT_EQ(block4, get_block(v, 1));
  EXPECT_EQ(block5, get_block(v, 2));
  EXPECT_EQ(block6, get_block(v, 3));
  EXPECT_EQ(empty_block, get_block(v, 4));
  EXPECT_EQ(empty_block, get_block(v, 5));

  v = pop_front(v);
  EXPECT_EQ(block4, get_block(v, 0));
  EXPECT_EQ(block5, get_block(v, 1));
  EXPECT_EQ(block6, get_block(v, 2));
  EXPECT_EQ(empty_block, get_block(v, 3));
  EXPECT_EQ(empty_block, get_block(v, 4));
  EXPECT_EQ(empty_block, get_block(v, 5));

  v = pop_front(v);
  EXPECT_EQ(block5, get_block(v, 0));
  EXPECT_EQ(block6, get_block(v, 1));
  EXPECT_EQ(empty_block, get_block(v, 2));
  EXPECT_EQ(empty_block, get_block(v, 3));
  EXPECT_EQ(empty_block, get_block(v, 4));
  EXPECT_EQ(empty_block, get_block(v, 5));

  v = pop_front(v);
  EXPECT_EQ(block6, get_block(v, 0));
  EXPECT_EQ(empty_block, get_block(v, 1));
  EXPECT_EQ(empty_block, get_block(v, 2));
  EXPECT_EQ(empty_block, get_block(v, 3));
  EXPECT_EQ(empty_block, get_block(v, 4));
  EXPECT_EQ(empty_block, get_block(v, 5));

  v = pop_front(v);
  EXPECT_EQ(empty_block, get_block(v, 0));
  EXPECT_EQ(empty_block, get_block(v, 1));
  EXPECT_EQ(empty_block, get_block(v, 2));
  EXPECT_EQ(empty_block, get_block(v, 3));
  EXPECT_EQ(empty_block, get_block(v, 4));
  EXPECT_EQ(empty_block, get_block(v, 5));

  v = pop_front(v);
  EXPECT_EQ(empty_block, get_block(v, 0));
  EXPECT_EQ(empty_block, get_block(v, 1));
  EXPECT_EQ(empty_block, get_block(v, 2));
  EXPECT_EQ(empty_block, get_block(v, 3));
  EXPECT_EQ(empty_block, get_block(v, 4));
  EXPECT_EQ(empty_block, get_block(v, 5));
}

TEST(TestVertex, pop_front_multi) {
  Vertex v =
      create(frect_color, block1, block2, block3, block4, block5, block6);
  v = pop_front(v, 3);
  EXPECT_EQ(block4, get_block(v, 0));
  EXPECT_EQ(block5, get_block(v, 1));
  EXPECT_EQ(block6, get_block(v, 2));
  EXPECT_EQ(empty_block, get_block(v, 3));
  EXPECT_EQ(empty_block, get_block(v, 4));
  EXPECT_EQ(empty_block, get_block(v, 5));
}

TEST(TestVertex, create_merged_onto_empty) {
  Vertex actual1      = create_merged(empty_vertex, v1);
  Vertex actual12     = create_merged(empty_vertex, v12);
  Vertex actual123    = create_merged(empty_vertex, v123);
  Vertex actual1234   = create_merged(empty_vertex, v1234);
  Vertex actual12345  = create_merged(empty_vertex, v12345);
  Vertex actual123456 = create_merged(empty_vertex, v123456);

  EXPECT_EQ(v1, actual1);
  EXPECT_EQ(v12, actual12);
  EXPECT_EQ(v123, actual123);
  EXPECT_EQ(v1234, actual1234);
  EXPECT_EQ(v12345, actual12345);
  EXPECT_EQ(v123456, actual123456);
}

TEST(TestVertex, create_merged_onto_one_block) {
  Vertex actual11      = create_merged(v1, v1);
  Vertex actual112     = create_merged(v1, v12);
  Vertex actual1123    = create_merged(v1, v123);
  Vertex actual11234   = create_merged(v1, v1234);
  Vertex actual112345  = create_merged(v1, v12345);
  Vertex actual1123456 = create_merged(v1, v123456);

  VertexMaker vm{frect_color};

  EXPECT_EQ(vm(block1, block1), actual11);
  EXPECT_EQ(vm(block1, block1, block2), actual112);
  EXPECT_EQ(vm(block1, block1, block2, block3), actual1123);
  EXPECT_EQ(vm(block1, block1, block2, block3, block4), actual11234);
  EXPECT_EQ(vm(block1, block1, block2, block3, block4, block5), actual112345);
  EXPECT_EQ(vm(block1, block1, block2, block3, block4, block5), actual1123456);
}

TEST(TestVertex, create_merged_onto_5_blocks) {
  Vertex actual1 = create_merged(v12345, v1);
  Vertex actual2 = create_merged(v12345, v12);
  Vertex actual3 = create_merged(v12345, v123);
  Vertex actual4 = create_merged(v12345, v1234);
  Vertex actual5 = create_merged(v12345, v12345);
  Vertex actual6 = create_merged(v12345, v123456);

  Vertex expected =
      create(frect_color, block1, block2, block3, block4, block5, block1);

  EXPECT_EQ(expected, actual1);
  EXPECT_EQ(expected, actual2);
  EXPECT_EQ(expected, actual3);
  EXPECT_EQ(expected, actual4);
  EXPECT_EQ(expected, actual5);
  EXPECT_EQ(expected, actual6);
}

TEST(TestVertex, to_external_name) {
  Transforms xforms;
  EXPECT_EQ("[':abcd]", to_external_short_name(v1234, xforms));
  EXPECT_EQ("[FROM:SOLID_RECTANGLE:abcd]", to_external_name(v1234, xforms));

  auto   twc_color  = to_final_color(color::Color::WILDCARD, RuleSide::TO);
  Vertex wct_vertex = vertex::create(twc_color, block::FinalBlock{1});
  EXPECT_EQ("[(:.]", to_external_short_name(wct_vertex, xforms));
  EXPECT_EQ("[TO:WILDCARD:.]", to_external_name(wct_vertex, xforms));
}

} // namespace vertex::test
