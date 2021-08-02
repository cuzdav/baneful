#include "AdjacencyMatrix.hpp"
#include "Color.hpp"
#include "GraphCreator.hpp"
#include "jsonlevelconfig.hpp"
#include "jsonutil.hpp"
#include "graph_utils.hpp"
#include "color_constants.hpp"

#include "boost/json.hpp"
#include "gtest/gtest.h"
#include <algorithm>

namespace test {

using namespace json;

using namespace std::literals;
using StrVec = std::vector<std::string>;

StrVec
vertex_names(boost::json::object lvl) {

  GraphCreator     gc(lvl);
  Vertices const & vertices = gc.get_vertices();

  std::vector<std::string> actual{vertices.names_begin(), vertices.names_end()};

  std::sort(begin(actual), end(actual));
  return actual;
}

StrVec
expected(StrVec args) {
  std::sort(begin(args), end(args));
  return args;
}

TEST(TestGraphCreator, simple_rule1) {
  auto lvl    = level(rules(from("a") = to("")));
  auto actual = vertex_names(lvl);

  EXPECT_EQ(expected({color::test::short_string::rect_fm + "a",
                      color::test::short_string::noth_to + "!"}),
            actual);
}

TEST(TestGraphCreator, simple_rule2) {
  auto lvl = level(rules(from("abc") = to("bb")));

  // "abc" is the id of the vertex starting at 'a'
  // "bc" is the id of the vertex starting at 'b'
  // "c" is the id of the vertex starting at 'c'
  auto actual = vertex_names(lvl);
  EXPECT_EQ(expected({color::test::short_string::rect_fm + "abc",
                      color::test::short_string::rect_fm + "bc",
                      color::test::short_string::rect_fm + "c",
                      color::test::short_string::rect_to + "bb",
                      color::test::short_string::rect_to + "b"}),
            actual);
}

TEST(TestGraphCreator, rule3_with_transform) {
  // clang-format off
  auto lvl = level(
                 rules(from("a") = to("b")),
                 type_overrides(
                     std::pair("a", rotating_colors("cd"))));
  // clang-format on

  auto actual = vertex_names(lvl);
  EXPECT_EQ(expected({color::test::short_string::cust_fm + "a",
                      color::test::short_string::rect_to + "b"}),
            actual);
}

TEST(TestGraphCreator, rule4_wc_backref_colors) {

  // clang-format off
  auto lvl = level(
                 rules(from(".") = to("121"),
                       from("a") = to("")));
  // clang-format on

  using namespace color::test::short_string;
  auto actual = vertex_names(lvl);
  EXPECT_EQ(expected({
                rect_fm + "a", noth_to + "!", wild_fm + ".",
                bref_to + "121", // id of the first 1
                bref_to + "21",  // id of the 2
                bref_to + "1",   // id of the 2nd 1 (in chain 121)
            }),
            actual);
}

TEST(TestGraphCreator, type_override_sets_up_custom_colors) {
  // clang-format off
  auto lvl = level(rules(from("a") = to("b")),
                   type_overrides(
                       std::pair("a", rotating_colors("cd")),
                       std::pair("b", rotating_colors("cde")),
                       std::pair("c", rotating_colors("abc"))));
  // clang-format on
  GraphCreator       gc(lvl);
  Transforms const & transforms = gc.get_transforms();
  StrVec             xform_names(transforms.begin(), transforms.end());
  std::sort(xform_names.begin(), xform_names.end());
  EXPECT_EQ(StrVec({"RotCol:abc", "RotCol:cd", "RotCol:cde"}), xform_names);

  color::FinalColor fr_a_color = transforms.to_color('a', RuleSide::FROM);
  color::FinalColor to_a_color = transforms.to_color('a', RuleSide::TO);
  color::FinalColor fr_b_color = transforms.to_color('b', RuleSide::FROM);
  color::FinalColor to_b_color = transforms.to_color('b', RuleSide::TO);
  color::FinalColor fr_c_color = transforms.to_color('c', RuleSide::FROM);
  color::FinalColor to_c_color = transforms.to_color('c', RuleSide::TO);

  using namespace color::test::short_string;
  EXPECT_EQ(cust_fm, to_short_string(fr_a_color));
  EXPECT_EQ(cust_to, to_short_string(to_a_color));
  EXPECT_EQ(cust2_fm, to_short_string(fr_b_color));
  EXPECT_EQ(cust2_to, to_short_string(to_b_color));
  EXPECT_EQ(cust3_fm, to_short_string(fr_c_color));
  EXPECT_EQ(cust3_to, to_short_string(to_c_color));
}

TEST(TestGraphCreator, proper_edges) {
  // clang-format off
  auto lvl = level(rules(from("a") = to("b", "cc"),
                         from("bb") = to(""),
                         from("c") = to("bb")
                  ));
  // clang-format on
  GraphCreator       gc(lvl);
  Transforms const & transforms = gc.get_transforms();
  Vertices const &   verts      = gc.get_vertices();

  // jsonutil::pretty_print(std::cout, lvl);

  auto has_edge = [&](auto vert_name1, auto vert_name2) {
    int from_idx = verts.index_of_internal_name(vert_name1);
    int to_idx   = verts.index_of_internal_name(vert_name2);
    return gc.has_edge(from_idx, to_idx);
  };

  using namespace color::test::short_string;

  EXPECT_TRUE(has_edge(rect_fm + "a", rect_to + "b"));
  EXPECT_TRUE(has_edge(rect_fm + "a", rect_to + "cc"));
  EXPECT_TRUE(has_edge(rect_fm + "bb", rect_fm + "b"));
  EXPECT_TRUE(has_edge(rect_fm + "b", noth_to + "!"));
  EXPECT_TRUE(has_edge(rect_to + "cc", rect_to + "c"));
  EXPECT_TRUE(has_edge(rect_fm + "c", rect_to + "bb"));
  EXPECT_TRUE(has_edge(rect_to + "bb", rect_to + "b"));

  EXPECT_FALSE(has_edge(rect_fm + "a", rect_fm + "bb"));
  EXPECT_FALSE(has_edge(rect_fm + "a", rect_fm + "b"));
  EXPECT_FALSE(has_edge(rect_fm + "a", noth_to + "!"));
  EXPECT_FALSE(has_edge(rect_fm + "a", rect_to + "bb"));
  EXPECT_FALSE(has_edge(rect_fm + "a", rect_to + "c"));

  EXPECT_FALSE(has_edge(rect_fm + "bb", rect_fm + "bb"));
  EXPECT_FALSE(has_edge(rect_fm + "bb", rect_fm + "a"));
  EXPECT_FALSE(has_edge(rect_fm + "bb", rect_fm + "c"));
  EXPECT_FALSE(has_edge(rect_fm + "bb", rect_to + "b"));
  EXPECT_FALSE(has_edge(rect_fm + "bb", rect_to + "cc"));
  EXPECT_FALSE(has_edge(rect_fm + "bb", rect_to + "c"));
  EXPECT_FALSE(has_edge(rect_fm + "bb", rect_to + "bb"));
  EXPECT_FALSE(has_edge(rect_fm + "bb", rect_to + "b"));
  EXPECT_FALSE(has_edge(rect_fm + "bb", noth_to + "!"));

  EXPECT_FALSE(has_edge(rect_fm + "b", rect_fm + "bb"));
  EXPECT_FALSE(has_edge(rect_fm + "b", rect_fm + "a"));
  EXPECT_FALSE(has_edge(rect_fm + "b", rect_to + "b"));
  EXPECT_FALSE(has_edge(rect_fm + "b", rect_fm + "c"));
  EXPECT_FALSE(has_edge(rect_fm + "b", rect_to + "cc"));
  EXPECT_FALSE(has_edge(rect_fm + "b", rect_to + "c"));
  EXPECT_FALSE(has_edge(rect_fm + "b", rect_fm + "bb"));
  EXPECT_FALSE(has_edge(rect_fm + "b", rect_fm + "b"));

  EXPECT_FALSE(has_edge(rect_fm + "c", rect_fm + "bb"));
  EXPECT_FALSE(has_edge(rect_fm + "c", rect_fm + "a"));
  EXPECT_FALSE(has_edge(rect_fm + "c", rect_fm + "b"));
  EXPECT_FALSE(has_edge(rect_fm + "c", rect_fm + "c"));
  EXPECT_FALSE(has_edge(rect_fm + "c", rect_to + "cc"));
  EXPECT_FALSE(has_edge(rect_fm + "c", rect_to + "c"));
  EXPECT_FALSE(has_edge(rect_fm + "c", noth_to + "!"));
  EXPECT_FALSE(has_edge(rect_fm + "c", rect_fm + "b"));
}

TEST(TestGraphCreator, using_common_vertices) {
  // clang-format off
  auto lvl = level(rules(from("a") = to("bc"),
                         from("b") = to("c")
                  ));
  // clang-format on
  GraphCreator       gc(lvl);
  Transforms const & transforms = gc.get_transforms();
  Vertices const &   verts      = gc.get_vertices();

  using namespace color::test::short_string;
  int frect_a  = verts.index_of_internal_name(rect_fm + "a");
  int frect_b  = verts.index_of_internal_name(rect_fm + "b");
  int trect_bc = verts.index_of_internal_name(rect_to + "bc");
  int trect_c  = verts.index_of_internal_name(rect_to + "c");

  EXPECT_TRUE(gc.has_edge(frect_a, trect_bc));
  EXPECT_TRUE(gc.has_edge(trect_bc, trect_c));
  EXPECT_TRUE(gc.has_edge(frect_b, trect_c));
}

TEST(TestGraphCreator, remove_vertex1) {
  auto                    lvl = level(rules(from("a") = to("bc")));
  GraphCreator            gc(lvl);
  Transforms const &      transforms = gc.get_transforms();
  AdjacencyMatrix const & adjmtx     = gc.get_adjacency_matrix();
  Vertices &              verts      = gc.get_vertices();

  std::map<std::string, int> name_idx_map;
  for (auto iter = verts.names_begin(), end = verts.names_end(); iter != end;
       ++iter) {
    name_idx_map[*iter] = verts.index_of_internal_name(*iter);
  }

  auto color      = to_final_color(color::Color::SOLID_RECTANGLE, RuleSide::TO);
  int  doomed_idx = verts.name_index_of("bc", color);

  vertex::Vertex orig_bbc = verts[1];
  vertex::Vertex orig_bc  = verts[2];
  vertex::Vertex orig_c   = verts[3];

  EXPECT_EQ(3, verts.names_size());
  EXPECT_TRUE(adjmtx.has_edge(0, 1));
  EXPECT_TRUE(adjmtx.has_edge(1, 2));
  EXPECT_EQ(1, verts.name_index_of("bc", color));
  EXPECT_EQ(2, verts.name_index_of("c", color));

  gc.compress_vertices();

  vertex::Vertex expected_bbc = add_block(orig_bbc, get_block(orig_bc, 0));
  EXPECT_EQ(2, verts.names_size());
  EXPECT_EQ(1, verts.name_index_of("bc", color));
  EXPECT_EQ(-1, verts.name_index_of("c", color));
  EXPECT_EQ(expected_bbc, verts[1]);
  EXPECT_TRUE(adjmtx.has_edge(0, 1));
  EXPECT_FALSE(adjmtx.has_edge(1, 2));
}

TEST(TestGraphCreator, dump_graph) {
  // clang-format off
  auto lvl = level(
      rules(from("a") = to("bcc"),
            from("b") = to("c")));
  // clang-format on
  GraphCreator gc(lvl);

  gc.get_adjacency_matrix().debug_dump();

  std::cout << utils::graph_to_string(gc) << std::endl;

  gc.compress_vertices();
  std::cout << "** AFTER **\n";
  gc.get_adjacency_matrix().debug_dump();
  std::cout << utils::graph_to_string(gc) << std::endl;
}

} // namespace test
