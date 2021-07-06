#include "GraphCreator.hpp"
#include "jsonlevelconfig.hpp"
#include "jsonutil.hpp"

#include "boost/json.hpp"
#include "gtest/gtest.h"
#include <algorithm>

std::string const TO_NOTH = "$";
std::string const FR_NOTH = "%";
std::string const TO_RECT = "&";
std::string const FR_RECT = "'";
std::string const TO_WILD = "(";
std::string const FR_WILD = ")";
std::string const TO_BREF = "*";
std::string const FR_BREF = "+";
std::string const TO_CUST = ","; // first custom.  Additionals are ASCII
std::string const FR_CUST = "-"; // sequentially following.

namespace p1::test {

using namespace std::literals;
using StrVec = std::vector<std::string>;

StrVec
vertex_names(boost::json::object lvl) {

  GraphCreator     gc(lvl);
  Verticies const &verticies = gc.get_verticies();

  std::vector<std::string> actual{verticies.names_begin(), verticies.names_end()};

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
  auto lvl    = level(rules(from("abc") = to("bb")));

  // "abc" is the id of the vertex starting at 'a'
  // "bc" is the id of the vertex starting at 'b'
  // "c" is the id of the vertex starting at 'c'
  auto actual = vertex_names(lvl);
  EXPECT_EQ(expected({FR_RECT + "abc", FR_RECT + "bc", FR_RECT + "c", TO_RECT + "bb",
                      TO_RECT + "b"}),
            actual);
}

TEST(TestGraphCreator, type_override_sets_up_custom_colors) {
  // clang-format off
  auto lvl = level(rules(from("a") = to("b")),
                   type_overrides(
                       std::pair("a", rotating_colors("cd"))));
  // clang-format on
  GraphCreator      gc(lvl);
  Transforms const &transforms = gc.get_transforms();
}

TEST(TestGraphCreator, rule3_with_transform) {
  // clang-format off
  auto lvl = level(
                 rules(from("a") = to("b")),
                 type_overrides(
                     std::pair("a", rotating_colors("cd"))));
  // clang-format on

  auto actual = vertex_names(lvl);
  EXPECT_EQ(expected({FR_CUST + "a", TO_RECT + "b"}), actual);
}

TEST(TestGraphCreator, rule4_wc_backref_colors) {

  // clang-format off
  auto lvl = level(
                 rules(from(".") = to("121"),
                       from("a") = to("")));
  // clang-format on

  auto actual = vertex_names(lvl);
  EXPECT_EQ(expected({
                FR_RECT + "a",
                TO_NOTH + "!",
                FR_WILD + ".",
                TO_BREF + "121",
                TO_BREF + "21",
                TO_BREF + "1",
            }),
            actual);
}

} // namespace p1::test
