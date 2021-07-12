#include "AdjacencyMatrix.hpp"
#include "GraphCreator.hpp"
#include "jsonlevelconfig.hpp"
#include "jsonutil.hpp"
#include "graphutils.hpp"

#include "boost/json.hpp"
#include "gtest/gtest.h"
#include <algorithm>

std::string const TO_NOTH  = "$";
std::string const FR_NOTH  = "%";
std::string const TO_RECT  = "&";
std::string const FR_RECT  = "'";
std::string const TO_WILD  = "(";
std::string const FR_WILD  = ")";
std::string const TO_BREF  = "*";
std::string const FR_BREF  = "+";
std::string const TO_CUST1 = ","; // first custom.  Additionals are ASCII
std::string const FR_CUST1 = "-"; // sequentially following.
std::string const TO_CUST2 = "."; // 2nd custom
std::string const FR_CUST2 = "/"; //
std::string const TO_CUST3 = "0"; // 3rd custom
std::string const FR_CUST3 = "1"; //

namespace test {

using namespace json;

using namespace std::literals;
using StrVec = std::vector<std::string>;

StrVec
vertex_names(boost::json::object lvl) {

  GraphCreator     gc(lvl);
  Vertices const & vertices = gc.get_vertices();

  std::vector<std::string> actual{vertices.names_begin(), vertices.names_end()};

  // 'a=FROM:RECT[a], $!=TO:NOTHING[!]
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

  EXPECT_EQ(expected({FR_RECT + "a", TO_NOTH + "!"}), actual);
}

TEST(TestGraphCreator, simple_rule2) {
  auto lvl = level(rules(from("abc") = to("bb")));

  // "abc" is the id of the vertex starting at 'a'
  // "bc" is the id of the vertex starting at 'b'
  // "c" is the id of the vertex starting at 'c'
  auto actual = vertex_names(lvl);
  EXPECT_EQ(expected({FR_RECT + "abc", FR_RECT + "bc", FR_RECT + "c",
                      TO_RECT + "bb", TO_RECT + "b"}),
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
  EXPECT_EQ(expected({FR_CUST1 + "a", TO_RECT + "b"}), actual);
}

TEST(TestGraphCreator, rule4_wc_backref_colors) {

  // clang-format off
  auto lvl = level(
                 rules(from(".") = to("121"),
                       from("a") = to("")));
  // clang-format on

  auto actual = vertex_names(lvl);
  EXPECT_EQ(expected({
                FR_RECT + "a", TO_NOTH + "!", FR_WILD + ".",
                TO_BREF + "121", // id of the first 1
                TO_BREF + "21",  // id of the 2
                TO_BREF + "1",   // id of the 2nd 1 (in chain 121)
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

  EXPECT_EQ(FR_CUST1[0], color_to_char(fr_a_color));
  EXPECT_EQ(TO_CUST1[0], color_to_char(to_a_color));
  EXPECT_EQ(FR_CUST2[0], color_to_char(fr_b_color));
  EXPECT_EQ(TO_CUST2[0], color_to_char(to_b_color));
  EXPECT_EQ(FR_CUST3[0], color_to_char(fr_c_color));
  EXPECT_EQ(TO_CUST3[0], color_to_char(to_c_color));
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

  EXPECT_TRUE(has_edge(FR_RECT + "a", TO_RECT + "b"));
  EXPECT_TRUE(has_edge(FR_RECT + "a", TO_RECT + "cc"));
  EXPECT_TRUE(has_edge(FR_RECT + "bb", FR_RECT + "b"));
  EXPECT_TRUE(has_edge(FR_RECT + "b", TO_NOTH + "!"));
  EXPECT_TRUE(has_edge(TO_RECT + "cc", TO_RECT + "c"));
  EXPECT_TRUE(has_edge(FR_RECT + "c", TO_RECT + "bb"));
  EXPECT_TRUE(has_edge(TO_RECT + "bb", TO_RECT + "b"));

  EXPECT_FALSE(has_edge(FR_RECT + "a", FR_RECT + "bb"));
  EXPECT_FALSE(has_edge(FR_RECT + "a", FR_RECT + "b"));
  EXPECT_FALSE(has_edge(FR_RECT + "a", TO_NOTH + "!"));
  EXPECT_FALSE(has_edge(FR_RECT + "a", TO_RECT + "bb"));
  EXPECT_FALSE(has_edge(FR_RECT + "a", TO_RECT + "c"));

  EXPECT_FALSE(has_edge(FR_RECT + "bb", FR_RECT + "bb"));
  EXPECT_FALSE(has_edge(FR_RECT + "bb", FR_RECT + "a"));
  EXPECT_FALSE(has_edge(FR_RECT + "bb", FR_RECT + "c"));
  EXPECT_FALSE(has_edge(FR_RECT + "bb", TO_RECT + "b"));
  EXPECT_FALSE(has_edge(FR_RECT + "bb", TO_RECT + "cc"));
  EXPECT_FALSE(has_edge(FR_RECT + "bb", TO_RECT + "c"));
  EXPECT_FALSE(has_edge(FR_RECT + "bb", TO_RECT + "bb"));
  EXPECT_FALSE(has_edge(FR_RECT + "bb", TO_RECT + "b"));
  EXPECT_FALSE(has_edge(FR_RECT + "bb", TO_NOTH + "!"));

  EXPECT_FALSE(has_edge(FR_RECT + "b", FR_RECT + "bb"));
  EXPECT_FALSE(has_edge(FR_RECT + "b", FR_RECT + "a"));
  EXPECT_FALSE(has_edge(FR_RECT + "b", TO_RECT + "b"));
  EXPECT_FALSE(has_edge(FR_RECT + "b", FR_RECT + "c"));
  EXPECT_FALSE(has_edge(FR_RECT + "b", TO_RECT + "cc"));
  EXPECT_FALSE(has_edge(FR_RECT + "b", TO_RECT + "c"));
  EXPECT_FALSE(has_edge(FR_RECT + "b", FR_RECT + "bb"));
  EXPECT_FALSE(has_edge(FR_RECT + "b", FR_RECT + "b"));

  EXPECT_FALSE(has_edge(FR_RECT + "c", FR_RECT + "bb"));
  EXPECT_FALSE(has_edge(FR_RECT + "c", FR_RECT + "a"));
  EXPECT_FALSE(has_edge(FR_RECT + "c", FR_RECT + "b"));
  EXPECT_FALSE(has_edge(FR_RECT + "c", FR_RECT + "c"));
  EXPECT_FALSE(has_edge(FR_RECT + "c", TO_RECT + "cc"));
  EXPECT_FALSE(has_edge(FR_RECT + "c", TO_RECT + "c"));
  EXPECT_FALSE(has_edge(FR_RECT + "c", TO_NOTH + "!"));
  EXPECT_FALSE(has_edge(FR_RECT + "c", FR_RECT + "b"));
}

TEST(TestGraphCreator, merging_edges) {
  // clang-format off
  auto lvl = level(rules(from("a") = to("bc"),
                         from("b") = to("c")
                  ));
  // clang-format on
  GraphCreator       gc(lvl);
  Transforms const & transforms = gc.get_transforms();
  Vertices const &   verts      = gc.get_vertices();

  int frect_a  = verts.index_of_internal_name(FR_RECT + "a");
  int frect_b  = verts.index_of_internal_name(FR_RECT + "b");
  int trect_bc = verts.index_of_internal_name(TO_RECT + "bc");
  int trect_c  = verts.index_of_internal_name(TO_RECT + "c");

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
