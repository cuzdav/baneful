#include "Color.hpp"
#include "enumutils.hpp"

#include "gtest/gtest.h"
#include <type_traits>

namespace p1::color::test {

static_assert(std::is_same_v<std::uint8_t, decltype(+(color::FinalColor{}))>);

TEST(TestColor, toUnderlying) {
  EXPECT_EQ(1, +Color::NOTHING);
  EXPECT_EQ(2, +Color::SOLID_RECTANGLE);
  EXPECT_EQ(3, +Color::WILDCARD);
  EXPECT_EQ(4, +Color::BACKREF);
  EXPECT_EQ(5, +Color::NEXT_CUSTOM);
}

TEST(TestColor, to_final_color_and_back) {
  using enum Color;
  using enum RuleSide;

  auto rect_to = std::pair(SOLID_RECTANGLE, TO);
  auto rect_fm = std::pair(SOLID_RECTANGLE, FROM);
  auto wild_to = std::pair(WILDCARD, TO);
  auto wild_fm = std::pair(WILDCARD, FROM);
  auto bref_to = std::pair(BACKREF, TO);
  auto bref_fm = std::pair(BACKREF, FROM);
  auto cust_to = std::pair(NEXT_CUSTOM, TO);
  auto cust_fm = std::pair(NEXT_CUSTOM, FROM);

  auto encode_decode = [](auto colorSidePair) {
    auto [color, side] = colorSidePair;
    FinalColor fc      = to_final_color(color, side);
    return std::pair(get_color(fc), get_rule_side(fc));
  };

  EXPECT_EQ(rect_to, encode_decode(rect_to));
  EXPECT_EQ(rect_fm, encode_decode(rect_fm));
  EXPECT_EQ(wild_to, encode_decode(wild_to));
  EXPECT_EQ(wild_fm, encode_decode(wild_fm));
  EXPECT_EQ(bref_to, encode_decode(bref_to));
  EXPECT_EQ(bref_fm, encode_decode(bref_fm));
  EXPECT_EQ(cust_to, encode_decode(cust_to));
  EXPECT_EQ(cust_fm, encode_decode(cust_fm));
}

TEST(TestColor, test_to_string) {
  using enum Color;
  using namespace std::literals;

  EXPECT_EQ("SOLID_RECTANGLE"s, to_string(SOLID_RECTANGLE));
  EXPECT_EQ("WILDCARD"s, to_string(WILDCARD));
  EXPECT_EQ("BACKREF"s, to_string(BACKREF));
  EXPECT_EQ("CUSTOM(5)"s, to_string(NEXT_CUSTOM));
}

TEST(TestColor, test_to_final_color_string_) {
  using enum Color;
  using enum RuleSide;
  using namespace std::literals;

  auto rect_to = to_string(to_final_color(SOLID_RECTANGLE, TO));
  auto rect_fm = to_string(to_final_color(SOLID_RECTANGLE, FROM));
  auto wild_to = to_string(to_final_color(WILDCARD, TO));
  auto wild_fm = to_string(to_final_color(WILDCARD, FROM));
  auto bref_to = to_string(to_final_color(BACKREF, TO));
  auto bref_fm = to_string(to_final_color(BACKREF, FROM));
  auto cust_to = to_string(to_final_color(NEXT_CUSTOM, TO));
  auto cust_fm = to_string(to_final_color(NEXT_CUSTOM, FROM));

  EXPECT_EQ("FinalColor(4):SOLID_RECTANGLE:TO"s, rect_to);
  EXPECT_EQ("FinalColor(5):SOLID_RECTANGLE:FROM"s, rect_fm);
  EXPECT_EQ("FinalColor(6):WILDCARD:TO"s, wild_to);
  EXPECT_EQ("FinalColor(7):WILDCARD:FROM"s, wild_fm);
  EXPECT_EQ("FinalColor(8):BACKREF:TO"s, bref_to);
  EXPECT_EQ("FinalColor(9):BACKREF:FROM"s, bref_fm);
  EXPECT_EQ("FinalColor(10):CUSTOM(5):TO"s, cust_to);
  EXPECT_EQ("FinalColor(11):CUSTOM(5):FROM"s, cust_fm);
}

} // namespace p1::color::test
