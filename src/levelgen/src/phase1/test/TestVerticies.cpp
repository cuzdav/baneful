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

TEST(TestVertices, GeneratedNames) {
  Vertices v;
  EXPECT_EQ(0, v.names_size());

  auto genidx0 = v.generate_unique_vertex_name();
  auto genidx1 = v.generate_unique_vertex_name();
  auto genidx2 = v.generate_unique_vertex_name();

  EXPECT_EQ(0, genidx0);
  EXPECT_EQ(1, genidx1);
  EXPECT_EQ(2, genidx2);

  EXPECT_EQ(3, v.names_size());

  EXPECT_EQ("_0", v.name_of(genidx0));
  EXPECT_EQ("_1", v.name_of(genidx1));
  EXPECT_EQ("_2", v.name_of(genidx2));
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

TEST(TestVertices, enums) {
  char              c = 'C';
  block::FinalBlock b{std::uint8_t(c)};
  char              result = +b;
  EXPECT_EQ('C', result);
}

TEST(TestVertices, Constants) {
  EXPECT_EQ(4, vertex::BitsPerBlock);
  EXPECT_EQ(5, vertex::BitsForColor);
  EXPECT_EQ(6, vertex::MaxBlocksPerVertex);
  EXPECT_EQ(15, vertex::BlockMask);
  EXPECT_EQ(31, vertex::ColorMask);
}

TEST(TestVertices, VertexEncodingColor) {
  vertex::Vertex v1 = vertex::create(fc_rect_from, empty_block);
  EXPECT_EQ(fc_rect_from, get_final_color(v1));
}

TEST(TestVertices, color_char_conversions) {
  using enum color::Color;
  auto final_color = to_final_color(SOLID_RECTANGLE, RuleSide::TO);
  auto iname       = Vertices::internal_name("a", final_color);

  // final color is color<<1 to make room for side. SOLID_RECTANGLE is 2, side
  // is 0, so fc is 4.  CHAR_OFFSET('\"')=34, thus 34 + 4 is 38 ().  Space char
  // added to put value into visible/printable character range
  EXPECT_EQ("&a", iname);
}

TEST(TestVertices, VertexEncodingGetBlockSingle) {
  block::FinalBlock block1{11};
  vertex::Vertex    v1 = vertex::create(fc_rect_from, block1);
  EXPECT_EQ(block1, get_block(v1, 0));
  EXPECT_EQ(empty_block, get_block(v1, 1));

  block::FinalBlock block2{5};
  vertex::Vertex    v2 = add_block(v1, block2);
  EXPECT_EQ(block1, get_block(v2, 0));
  EXPECT_EQ(block2, get_block(v2, 1));
  EXPECT_EQ(empty_block, get_block(v2, 2));

  block::FinalBlock block3{15};
  vertex::Vertex    v3 = add_block(v2, block3);
  EXPECT_EQ(block1, get_block(v3, 0));
  EXPECT_EQ(block2, get_block(v3, 1));
  EXPECT_EQ(block3, get_block(v3, 2));
  EXPECT_EQ(empty_block, get_block(v3, 3));

  block::FinalBlock block4{14};
  vertex::Vertex    v4 = add_block(v3, block4);
  EXPECT_EQ(block1, get_block(v4, 0));
  EXPECT_EQ(block2, get_block(v4, 1));
  EXPECT_EQ(block3, get_block(v4, 2));
  EXPECT_EQ(block4, get_block(v4, 3));
  EXPECT_EQ(empty_block, get_block(v4, 4));

  block::FinalBlock block5{8};
  vertex::Vertex    v5 = add_block(v4, block5);
  EXPECT_EQ(block1, get_block(v5, 0));
  EXPECT_EQ(block2, get_block(v5, 1));
  EXPECT_EQ(block3, get_block(v5, 2));
  EXPECT_EQ(block4, get_block(v5, 3));
  EXPECT_EQ(block5, get_block(v5, 4));
  EXPECT_EQ(empty_block, get_block(v5, 5));

  block::FinalBlock block6{3};
  vertex::Vertex    v6 = add_block(v5, block6);
  EXPECT_EQ(block1, get_block(v6, 0));
  EXPECT_EQ(block2, get_block(v6, 1));
  EXPECT_EQ(block3, get_block(v6, 2));
  EXPECT_EQ(block4, get_block(v6, 3));
  EXPECT_EQ(block5, get_block(v6, 4));
  EXPECT_EQ(block6, get_block(v6, 5));
}

TEST(TestVertices, VertexEncodingGetBlocks) {
  block::FinalBlock block1{11};
  vertex::Vertex    v1 = vertex::create(fc_rect_from, block1);

  auto blocks = get_blocks(v1);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(empty_block, blocks[1]);

  block::FinalBlock block2{5};
  vertex::Vertex    v2 = add_block(v1, block2);
  blocks               = get_blocks(v2);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(empty_block, blocks[2]);

  block::FinalBlock block3{15};
  vertex::Vertex    v3 = add_block(v2, block3);
  blocks               = get_blocks(v3);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(empty_block, blocks[3]);

  block::FinalBlock block4{14};
  vertex::Vertex    v4 = add_block(v3, block4);
  blocks               = get_blocks(v4);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(block4, blocks[3]);
  EXPECT_EQ(empty_block, blocks[4]);

  block::FinalBlock block5{8};
  vertex::Vertex    v5 = add_block(v4, block5);
  blocks               = get_blocks(v5);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(block4, blocks[3]);
  EXPECT_EQ(block5, blocks[4]);
  EXPECT_EQ(empty_block, blocks[5]);

  block::FinalBlock block6{3};
  vertex::Vertex    v6 = add_block(v5, block6);
  blocks               = get_blocks(v6);
  EXPECT_EQ(block1, blocks[0]);
  EXPECT_EQ(block2, blocks[1]);
  EXPECT_EQ(block3, blocks[2]);
  EXPECT_EQ(block4, blocks[3]);
  EXPECT_EQ(block5, blocks[4]);
  EXPECT_EQ(block6, blocks[5]);
}
