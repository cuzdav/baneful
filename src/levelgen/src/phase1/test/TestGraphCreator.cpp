#include "GraphCreator.hpp"
#include "jsonlevelconfig.hpp"
#include "jsonutil.hpp"

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

namespace p1::test {

using namespace std::literals;
using StrVec = std::vector<std::string>;

StrVec
vertex_names(boost::json::object lvl) {

  GraphCreator      gc(lvl);
  Verticies const & verticies = gc.get_verticies();

  std::vector<std::string> actual{verticies.names_begin(),
                                  verticies.names_end()};

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

  color::FinalColor fr_a_color = transforms.get_color('a', RuleSide::FROM);
  color::FinalColor to_a_color = transforms.get_color('a', RuleSide::TO);
  color::FinalColor fr_b_color = transforms.get_color('b', RuleSide::FROM);
  color::FinalColor to_b_color = transforms.get_color('b', RuleSide::TO);
  color::FinalColor fr_c_color = transforms.get_color('c', RuleSide::FROM);
  color::FinalColor to_c_color = transforms.get_color('c', RuleSide::TO);

  EXPECT_EQ(FR_CUST1[0], color_to_char(fr_a_color));
  EXPECT_EQ(TO_CUST1[0], color_to_char(to_a_color));
  EXPECT_EQ(FR_CUST2[0], color_to_char(fr_b_color));
  EXPECT_EQ(TO_CUST2[0], color_to_char(to_b_color));
  EXPECT_EQ(FR_CUST3[0], color_to_char(fr_c_color));
  EXPECT_EQ(TO_CUST3[0], color_to_char(to_c_color));
}

} // namespace p1::test
