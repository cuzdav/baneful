#include <string_view>
#include <gtest/gtest.h>

#include "Color.hpp"
#include "Verticies.hpp"

using namespace p1;
using namespace std::literals;

TEST(TestVerticies, BasicInterface) {
  Verticies v;
  EXPECT_EQ(v.end(), v.begin());
  EXPECT_EQ(0, v.size());

  Verticies const v2;
  EXPECT_EQ(v2.end(), v2.begin());
  EXPECT_EQ(0, v2.size());
}

TEST(TestVerticies, GeneratedNames) {
  Verticies v;
  EXPECT_EQ(0, v.size());

  auto genidx0 = v.generate_unique_vertex_name();
  auto genidx1 = v.generate_unique_vertex_name();
  auto genidx2 = v.generate_unique_vertex_name();

  EXPECT_EQ(0, genidx0);
  EXPECT_EQ(1, genidx1);
  EXPECT_EQ(2, genidx2);

  EXPECT_EQ(3, v.size());

  EXPECT_EQ("_0", v.name_of(genidx0));
  EXPECT_EQ("_1", v.name_of(genidx1));
  EXPECT_EQ("_2", v.name_of(genidx2));
}

TEST(TestVerticies, AddVertexSingleDefault) {
  Verticies v;

  int idx = v.add_vertex_single("abc"sv, '\0', color::Color::DEFAULT);
  EXPECT_EQ(1, v.size());

  auto abc_internal = Verticies::internal_name("abc", color::Color::DEFAULT);
  EXPECT_EQ(abc_internal, v.name_of(idx));

  int idx2 = v.add_vertex_single("abc"sv, '\0', color::Color::DEFAULT);
  EXPECT_EQ(1, v.size());
  EXPECT_EQ(idx, idx2);
}

TEST(TestVerticies, AddVertexSingle_SameNameByColor) {
  Verticies v;

  int idx = v.add_vertex_single("abc"sv, '\0', color::Color::DEFAULT);
  int idx2 = v.add_vertex_single("abc"sv, '\0', color::Color::WILDCARD);
  EXPECT_EQ(2, v.size());
  EXPECT_EQ(0, idx);
  EXPECT_EQ(1, idx2);
}
