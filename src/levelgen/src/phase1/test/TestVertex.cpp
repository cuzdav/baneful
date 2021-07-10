#include "gtest/gtest.h"
#include "Color.hpp"
#include "Transforms.hpp"
#include "Vertex.hpp"
#include "Vertices.hpp"

namespace p1::vertex::test {

using enum color::Color;
using enum RuleSide;

using FinalColor = color::FinalColor;

constexpr FinalColor frect_color = to_final_color(SOLID_RECTANGLE, FROM),
                     trect_color = to_final_color(SOLID_RECTANGLE, TO),
                     fwild_color = to_final_color(WILDCARD, FROM),
                     twild_color = to_final_color(WILDCARD, TO),
                     fbref_color = to_final_color(BACKREF, FROM),
                     tbref_color = to_final_color(BACKREF, TO);

static constexpr block::FinalBlock empty_block = block::FinalBlock{'\0'};
block::FinalBlock                  block1      = block::FinalBlock{1};
block::FinalBlock                  block2      = block::FinalBlock{2};
block::FinalBlock                  block3      = block::FinalBlock{3};
block::FinalBlock                  block4      = block::FinalBlock{4};
block::FinalBlock                  block5      = block::FinalBlock{5};
block::FinalBlock                  block6      = block::FinalBlock{6};

Transforms tr;

TEST(TestVertex, enums) {
  char              c = 'C';
  block::FinalBlock b{std::uint8_t(c)};
  char              result = +b;
  EXPECT_EQ('C', result);
}

TEST(TestVertex, Constants) {
  EXPECT_EQ(4, vertex::BitsPerBlock);
  EXPECT_EQ(5, vertex::BitsForColor);
  EXPECT_EQ(6, vertex::MaxBlocksPerVertex);
  EXPECT_EQ(15, vertex::BlockMask);
  EXPECT_EQ(31, vertex::ColorMask);
}

TEST(TestVertex, VertexEncodingColor) {
  vertex::Vertex v1 = vertex::create(frect_color, empty_block);
  EXPECT_EQ(frect_color, get_final_color(v1));
}

TEST(TestVertex, color_char_conversions) {
  using enum color::Color;
  auto final_color = to_final_color(SOLID_RECTANGLE, TO);
  auto iname       = Vertices::internal_name("a", final_color);

  // final color is color<<1 to make room for side. SOLID_RECTANGLE is 2, side
  // is 0, so fc is 4.  CHAR_OFFSET('\"')=34, thus 34 + 4 is 38 ().  Space char
  // added to put value into visible/printable character range
  EXPECT_EQ("&a", iname);
}

TEST(TestVertex, VertexEncodingGetBlockSingle) {
  vertex::Vertex v1 = vertex::create(frect_color, block1);
  EXPECT_EQ(block1, get_block(v1, 0));
  EXPECT_EQ(empty_block, get_block(v1, 1));

  vertex::Vertex v2 = add_block(v1, block2);
  EXPECT_EQ(block1, get_block(v2, 0));
  EXPECT_EQ(block2, get_block(v2, 1));
  EXPECT_EQ(empty_block, get_block(v2, 2));

  vertex::Vertex v3 = add_block(v2, block3);
  EXPECT_EQ(block1, get_block(v3, 0));
  EXPECT_EQ(block2, get_block(v3, 1));
  EXPECT_EQ(block3, get_block(v3, 2));
  EXPECT_EQ(empty_block, get_block(v3, 3));

  vertex::Vertex v4 = add_block(v3, block4);
  EXPECT_EQ(block1, get_block(v4, 0));
  EXPECT_EQ(block2, get_block(v4, 1));
  EXPECT_EQ(block3, get_block(v4, 2));
  EXPECT_EQ(block4, get_block(v4, 3));
  EXPECT_EQ(empty_block, get_block(v4, 4));

  vertex::Vertex v5 = add_block(v4, block5);
  EXPECT_EQ(block1, get_block(v5, 0));
  EXPECT_EQ(block2, get_block(v5, 1));
  EXPECT_EQ(block3, get_block(v5, 2));
  EXPECT_EQ(block4, get_block(v5, 3));
  EXPECT_EQ(block5, get_block(v5, 4));
  EXPECT_EQ(empty_block, get_block(v5, 5));

  vertex::Vertex v6 = add_block(v5, block6);
  EXPECT_EQ(block1, get_block(v6, 0));
  EXPECT_EQ(block2, get_block(v6, 1));
  EXPECT_EQ(block3, get_block(v6, 2));
  EXPECT_EQ(block4, get_block(v6, 3));
  EXPECT_EQ(block5, get_block(v6, 4));
  EXPECT_EQ(block6, get_block(v6, 5));
}

TEST(TestVertex, VertexEncodingGetBlocks) {
  vertex::Vertex v1 = vertex::create(frect_color, block1);

  auto blocks = get_blocks(v1);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(empty_block, blocks[1]);

  vertex::Vertex v2 = add_block(v1, block2);
  blocks            = get_blocks(v2);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(empty_block, blocks[2]);

  vertex::Vertex v3 = add_block(v2, block3);
  blocks            = get_blocks(v3);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(empty_block, blocks[3]);

  vertex::Vertex v4 = add_block(v3, block4);
  blocks            = get_blocks(v4);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(block4, blocks[3]);
  EXPECT_EQ(empty_block, blocks[4]);

  vertex::Vertex v5 = add_block(v4, block5);
  blocks            = get_blocks(v5);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(block4, blocks[3]);
  EXPECT_EQ(block5, blocks[4]);
  EXPECT_EQ(empty_block, blocks[5]);

  vertex::Vertex v6 = add_block(v5, block6);
  blocks            = get_blocks(v6);
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

  Vertex v1 = vertex::create(frect_color, empty_block),
         v2 = vertex::create(trect_color, empty_block),
         v3 = vertex::create(fwild_color, empty_block),
         v4 = vertex::create(twild_color, empty_block),
         v5 = vertex::create(fbref_color, empty_block),
         v6 = vertex::create(tbref_color, empty_block);

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

TEST(TestVertex, createMerged) {
}

} // namespace p1::vertex::test
