
#include "Color.hpp"
#include "Graph.hpp"
#include "GraphCreator.hpp"
#include "jsonutil.hpp"
#include "Vertex.hpp"
#include "Vertices.hpp"

#include "jsonlevelconfig.hpp"

#include <gtest/gtest.h>
#include <boost/json.hpp>

using namespace test::json;
using namespace boost;

bool
test_isomorphism(json::object level1, json::object level2) {
  Graph graph1 =
      GraphCreator(level1).compress_vertices().group_by_colors().create();
  Graph graph2 =
      GraphCreator(level2).compress_vertices().group_by_colors().create();

  return graph1.check_isomorphism(graph2);
}

TEST(TestGraph, test_simplist_isoporphism) {
  auto lvl = level(rules(from("a") = to("b")));
  EXPECT_TRUE(test_isomorphism(lvl, lvl));
}

TEST(TestGraph, test_simple_isoporphism) {
  auto lvl1 = level(rules(from("a") = to("b")));
  auto lvl2 = level(rules(from("b") = to("c")));
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_simple_revered_identical_isoporphism) {
  // clang-format off
  auto lvl1 = level(rules(from("a") = to("b"),
                          from("b") = to("c")));
  auto lvl2 = level(rules(from("b") = to("c"),
                          from("a") = to("b")));
  // clang-format on
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_simple_not_isoporphism) {
  // clang-format off
  auto lvl1 = level(rules(from("a") = to("b"),
                          from("b") = to("c")));
  auto lvl2 = level(rules(from("b") = to("c"),
                          from("a") = to("c")));
  // clang-format on
  EXPECT_FALSE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_simple_mixed_color_isoporphism) {
  // clang-format off
  auto lvl1 = level(rules(from("a") = to("b"),
                          from(".") = to("c")));
  auto lvl2 = level(rules(from(".") = to("c"),
                          from("a") = to("b")));
  // clang-format on
  EXPECT_TRUE(test_isomorphism(lvl1, lvl2));
}

TEST(TestGraph, test_simple_mixed_color_isoporphism2) {
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
