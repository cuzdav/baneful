
#include "AdjacencyMatrix.hpp"
#include "Color.hpp"
#include "Graph.hpp"
#include "GraphCreator.hpp"
#include "jsonutil.hpp"
#include "Vertex.hpp"
#include "Vertices.hpp"

#include "color_constants.hpp"
#include "jsonlevelconfig.hpp"

#include <gtest/gtest.h>
#include <boost/json.hpp>

using namespace test::json;
using namespace boost;
using enum vertex::VertexRole;

auto const block1 = block::FinalBlock{1};

bool
test_isomorphism(json::object level1, json::object level2, bool dump = false) {
  if (dump) {
    std::cout << "Creating Graph1\n";
  }
  Graph graph1 =
      GraphCreator(level1).compress_vertices().group_by_colors().create();
  if (dump) {
    std::cout << "Creating Graph2\n";
  }
  Graph graph2 =
      GraphCreator(level2).compress_vertices().group_by_colors().create();

  if (dump) {
    graph1.dump("Graph1");
    graph2.dump("Graph2");
  }
  return graph1.check_isomorphism(graph2);
}

TEST(TestGraph, test_populate_colorgroups) {
  Vertices v;
  using namespace color::test;
  v.add_vertex_single("a", block1, fc::rect_fm, INTERNAL);
  v.add_vertex_single("b", block1, fc::rect_to, INTERNAL);
  v.add_vertex_single("c", block1, fc::rect_to, INTERNAL);
  v.add_vertex_single("d", block1, fc::noth_to, INTERNAL);
  Graph victim(std::move(v), matrix::AdjacencyMatrix{0});
  ASSERT_EQ(1, victim.permutable_block_ranges().size());
  EXPECT_EQ(std::pair(1, 3), victim.permutable_block_ranges()[0]);
}

TEST(TestGraph, test_populate_colorgroups_start) {
  Vertices v;
  using namespace color::test;
  v.add_vertex_single("b", block1, fc::rect_to, INTERNAL);
  v.add_vertex_single("c", block1, fc::rect_to, INTERNAL);
  v.add_vertex_single("d", block1, fc::noth_to, INTERNAL);
  Graph victim(std::move(v), matrix::AdjacencyMatrix{0});
  ASSERT_EQ(1, victim.permutable_block_ranges().size());
  EXPECT_EQ(std::pair(0, 2), victim.permutable_block_ranges()[0]);
}

TEST(TestGraph, test_populate_colorgroups_end) {
  Vertices v;
  using namespace color::test;
  v.add_vertex_single("d", block1, fc::noth_to, INTERNAL);
  v.add_vertex_single("b", block1, fc::rect_to, INTERNAL);
  v.add_vertex_single("c", block1, fc::rect_to, INTERNAL);
  Graph victim(std::move(v), matrix::AdjacencyMatrix{0});
  ASSERT_EQ(1, victim.permutable_block_ranges().size());
  EXPECT_EQ(std::pair(1, 3), victim.permutable_block_ranges()[0]);
}

TEST(TestGraph, test_populate_colorgroups_multi_with_gaps) {
  Vertices v;
  using namespace color::test;
  v.add_vertex_single("a", block1, fc::rect_fm, INTERNAL);
  v.add_vertex_single("b", block1, fc::rect_fm, INTERNAL);
  v.add_vertex_single("c", block1, fc::noth_to, INTERNAL);
  v.add_vertex_single("d", block1, fc::bref_to, INTERNAL);
  v.add_vertex_single("e", block1, fc::bref_to, INTERNAL);
  v.add_vertex_single("ee", block1, fc::bref_to, INTERNAL);
  v.add_vertex_single("c", block1, fc::wild_to, INTERNAL);
  v.add_vertex_single("f", block1, fc::rect_to, INTERNAL);
  v.add_vertex_single("g", block1, fc::rect_to, INTERNAL);
  Graph victim(std::move(v), matrix::AdjacencyMatrix{0});
  ASSERT_EQ(3, victim.permutable_block_ranges().size());
  EXPECT_EQ(std::pair(0, 2), victim.permutable_block_ranges()[0]);
  EXPECT_EQ(std::pair(3, 6), victim.permutable_block_ranges()[1]);
  EXPECT_EQ(std::pair(7, 9), victim.permutable_block_ranges()[2]);
}

TEST(TestGraph, test_populate_colorgroups_none) {
  Vertices v;
  using namespace color::test;
  v.add_vertex_single("a", block1, fc::rect_fm, INTERNAL);
  v.add_vertex_single("b", block1, fc::rect_to, INTERNAL);
  v.add_vertex_single("d", block1, fc::noth_to, INTERNAL);
  Graph victim(std::move(v), matrix::AdjacencyMatrix{0});
  ASSERT_EQ(0, victim.permutable_block_ranges().size());
}

TEST(TestGraph, test_simplist_isomorphism) {
  auto lvl = level(rules(from("a") = to("b")));
  EXPECT_TRUE(test_isomorphism(lvl, lvl));
}

TEST(TestGraph, test_simple_isomorphism) {
  auto lvl1 = level(rules(from("a") = to("b")));
  auto lvl2 = level(rules(from("b") = to("c")));
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_simple_reversed_identical_isomorphism) {
  // clang-format off
  auto lvl1 = level(rules(from("a") = to("b"),
                          from("b") = to("c")));
  auto lvl2 = level(rules(from("b") = to("c"),
                          from("a") = to("b")));
  // clang-format on
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_simple_not_isomorphism) {
  // clang-format off
  auto lvl1 = level(rules(from("a") = to("b"),
                          from("b") = to("c")));
  auto lvl2 = level(rules(from("b") = to("c"),
                          from("a") = to("c")));
  // clang-format on
  EXPECT_FALSE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_simple_mixed_color_isomorphism) {
  // clang-format off
  auto lvl1 = level(rules(from("a") = to("b"),
                          from(".") = to("c")));
  auto lvl2 = level(rules(from(".") = to("c"),
                          from("a") = to("b")));
  // clang-format on
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_simple_mixed_color_isomorphism2) {
  // clang-format off
  auto lvl1 = level(rules(from("a") = to("b"),
                          from(".") = to("c")));

  auto lvl2 = level(rules(from(".") = to("c"),
                          from("b") = to("a")));
  // clang-format on
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_complex_april1_level) {
  //  // clang-format off
  auto lvl1 = level(rules(from("a")    = to("bc"),
                          from("abcd") = to(""),
                          from("bc")   = to("bcd", "c"),
                          from("cbc")  = to("ab"),
                          from("d")    = to("a", "db"),
                          from("db")   = to("b")));

  auto lvl2 = level(rules(from("a")    = to("bc"),
                          from("abcd") = to(""),
                          from("bc")   = to("bcd", "c"),
                          from("cbc")  = to("ab"),
                          from("d")    = to("a", "db"),
                          from("db")   = to("b")));
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_complex_april1_level_reordered) {
  // clang-format off
  auto lvl1 = level(rules(from("a")    = to("bc"),
                          from("abcd") = to(""),
                          from("bc")   = to("bcd", "c"),
                          from("cbc")  = to("ab"),
                          from("d")    = to("a", "db"),
                          from("db")   = to("b")));

  auto lvl2 = level(rules(from("a")    = to("bc"),
                          from("db")   = to("b"),
                          from("d")    = to("a", "db"),
                          from("bc")   = to("bcd", "c"),
                          from("cbc")  = to("ab"),
                          from("abcd") = to("")

  ));
  // clang-format on
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_complex_april1_level_reordered_and_relabeled_blocks) {
  // clang-format off
  auto lvl1 = level(rules(from("a")    = to("bc"),
                          from("abcd") = to(""),
                          from("bc")   = to("bcd", "c"),
                          from("cbc")  = to("ab"),
                          from("d")    = to("a", "db"),
                          from("db")   = to("b")));


  auto lvl2 = level(rules(from("e")    = to("ca"),
                          from("bc")   = to("c"),
                          from("b")    = to("e", "bc"),
                          from("ca")   = to("cab", "a"),
                          from("aca")  = to("ec"),
                          from("ecab") = to("")

  ));
  // clang-format on
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph,
     test_complex_april1_level_reordered_and_relabeled_blocks_DIFFERS) {
  // clang-format off
  auto lvl1 = level(rules(from("a")    = to("bc"),
                          from("abcd") = to(""),
                          from("bc")   = to("bcd", "c"),
                          from("cbc")  = to("ab"),
                          from("d")    = to("a", "db"),
                          from("db")   = to("b")));


  auto lvl2 = level(rules(from("e")    = to("ca"),
                          from("bc")   = to(""), // was "c"
                          from("b")    = to("e", "bc"),
                          from("ca")   = to("cab", "a"),
                          from("aca")  = to("ec"),
                          from("ecab") = to("")

  ));
  // clang-format on
  EXPECT_FALSE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_simple_compressed_isomorphism) {
  //  // clang-format off
  auto lvl1 = level(rules(from("bc") = to("bcd")));
  auto lvl2 = level(rules(from("ac") = to("acd")));

  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, careful_reduce) {
  // These two from standard.json were falsely flagged as isomorphism, so it is
  // now a test case.

  // clang-format off
  auto lvl1 = level(rules(from("ab") = to("b"),
                          from("b")  = to("")));
  auto lvl2 = level(rules(from("abbb") = to(""),
                          from("b")    = to("bb", "")));
  // clang-format off

  EXPECT_FALSE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, bad_subsumption) {
  // Before start bits were maintained, the following completely folded b->""
  // into the tail of ab->"" and the two levels below were indistinguishable.
  // But now in level1 "a" and "b" both have start bits, while in lvl2 only "a"
  // has the start bit.

  // clang-format off
  auto lvl1 = level(rules(from("ab") = to(""),
                          from("b")  = to("")));

  auto lvl2 = level(rules(from("ab") = to("")));
  // clang-format off

  EXPECT_FALSE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_level_reordered) {
  // clang-format off
  auto lvl1 = level(rules(from("abc") = to(""),
                          from("cb")  = to("a")
                          ));

  auto lvl2 = level(rules(from("cb")  = to("a"),
                          from("abc") = to("")
                          ));
  // clang-format on
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}
