#include "Color.hpp"
#include "enum_utils.hpp"
#include "color_constants.hpp"
#include "gtest/gtest.h"
#include <type_traits>

namespace color::test {

static_assert(std::is_same_v<std::uint8_t, decltype(+(color::FinalColor{}))>);

TEST(TestColor, toUnderlying) {
  using enum Color;
  EXPECT_EQ(1, +NOTHING);
  EXPECT_EQ(2, +SOLID_RECTANGLE);
  EXPECT_EQ(3, +WILDCARD);
  EXPECT_EQ(4, +BACKREF);
  EXPECT_EQ(5, +ROTATING_COLORS);
  EXPECT_EQ(6, +NEXT_CUSTOM);
}

TEST(TestColor, to_final_color_and_back) {
  using enum Color;
  using enum RuleSide;

  auto decode = [](auto fcolor) {
    return std::pair(get_color(fcolor), get_rule_side(fcolor));
  };

  EXPECT_EQ(std::pair(SOLID_RECTANGLE, TO), decode(fc::rect_to));
  EXPECT_EQ(std::pair(SOLID_RECTANGLE, FROM), decode(fc::rect_fm));
  EXPECT_EQ(std::pair(WILDCARD, TO), decode(fc::wild_to));
  EXPECT_EQ(std::pair(WILDCARD, FROM), decode(fc::wild_fm));
  EXPECT_EQ(std::pair(BACKREF, TO), decode(fc::bref_to));
  EXPECT_EQ(std::pair(BACKREF, FROM), decode(fc::bref_fm));
  EXPECT_EQ(std::pair(ROTATING_COLORS, TO), decode(fc::rotc_to));
  EXPECT_EQ(std::pair(ROTATING_COLORS, FROM), decode(fc::rotc_fm));
  EXPECT_EQ(std::pair(NEXT_CUSTOM, TO), decode(fc::cust_to));
  EXPECT_EQ(std::pair(NEXT_CUSTOM, FROM), decode(fc::cust_fm));
}

TEST(TestColor, test_color_to_string) {
  using enum Color;
  using namespace std::literals;

  EXPECT_EQ("SOLID_RECTANGLE"s, to_string(SOLID_RECTANGLE));
  EXPECT_EQ("WILDCARD"s, to_string(WILDCARD));
  EXPECT_EQ("BACKREF"s, to_string(BACKREF));
  EXPECT_EQ("ROTATING_COLORS"s, to_string(ROTATING_COLORS));
  EXPECT_EQ("CUSTOM+0"s, to_string(NEXT_CUSTOM));
}

TEST(TestColor, test_final_color_to_long_string) {
  using enum Color;
  using enum RuleSide;
  using namespace std::literals;
  using namespace fc;
  EXPECT_EQ("FinalColor(4):SOLID_RECTANGLE:TO"s, to_long_string(rect_to));
  EXPECT_EQ("FinalColor(5):SOLID_RECTANGLE:FROM"s, to_long_string(rect_fm));
  EXPECT_EQ("FinalColor(6):WILDCARD:TO"s, to_long_string(wild_to));
  EXPECT_EQ("FinalColor(7):WILDCARD:FROM"s, to_long_string(wild_fm));
  EXPECT_EQ("FinalColor(8):BACKREF:TO"s, to_long_string(bref_to));
  EXPECT_EQ("FinalColor(9):BACKREF:FROM"s, to_long_string(bref_fm));
  EXPECT_EQ("FinalColor(10):ROTATING_COLORS:TO"s, to_long_string(rotc_to));
  EXPECT_EQ("FinalColor(11):ROTATING_COLORS:FROM"s, to_long_string(rotc_fm));
  EXPECT_EQ("FinalColor(12):CUSTOM+0:TO"s, to_long_string(cust_to));
  EXPECT_EQ("FinalColor(13):CUSTOM+0:FROM"s, to_long_string(cust_fm));
  EXPECT_EQ("FinalColor(14):CUSTOM+1:TO"s, to_long_string(cust2_to));
  EXPECT_EQ("FinalColor(15):CUSTOM+1:FROM"s, to_long_string(cust2_fm));
}

TEST(TestColor, test_final_color_to_string) {
  using namespace fc;
  EXPECT_EQ("TO:SOLID_RECTANGLE"s, to_string(rect_to));
  EXPECT_EQ("FROM:SOLID_RECTANGLE"s, to_string(rect_fm));
  EXPECT_EQ("TO:WILDCARD"s, to_string(wild_to));
  EXPECT_EQ("FROM:WILDCARD"s, to_string(wild_fm));
  EXPECT_EQ("TO:BACKREF"s, to_string(bref_to));
  EXPECT_EQ("FROM:BACKREF"s, to_string(bref_fm));
  EXPECT_EQ("TO:CUSTOM+0"s, to_string(cust_to));
  EXPECT_EQ("FROM:CUSTOM+0"s, to_string(cust_fm));
  EXPECT_EQ("TO:CUSTOM+1"s, to_string(cust2_to));
  EXPECT_EQ("FROM:CUSTOM+1"s, to_string(cust2_fm));
}

TEST(TestColor, test_color_to_short_string) {
  using namespace fc;
  EXPECT_EQ("TR"s, to_short_string(rect_to));
  EXPECT_EQ("FR"s, to_short_string(rect_fm));
  EXPECT_EQ("T."s, to_short_string(wild_to));
  EXPECT_EQ("F."s, to_short_string(wild_fm));
  EXPECT_EQ("TB"s, to_short_string(bref_to));
  EXPECT_EQ("FB"s, to_short_string(bref_fm));
  EXPECT_EQ("TU"s, to_short_string(cust_to));
  EXPECT_EQ("FU"s, to_short_string(cust_fm));
  EXPECT_EQ("TV"s, to_short_string(cust2_to));
  EXPECT_EQ("FV"s, to_short_string(cust2_fm));
  EXPECT_EQ("TW"s, to_short_string(cust3_to));
  EXPECT_EQ("FW"s, to_short_string(cust3_fm));
}

TEST(TestColor, test_short_string_to_color) {
  using enum Color;

  EXPECT_EQ(SOLID_RECTANGLE, get_color(short_string_to_color("TR"s)));
  EXPECT_EQ(SOLID_RECTANGLE, get_color(short_string_to_color("FR"s)));
  EXPECT_EQ(WILDCARD, get_color(short_string_to_color("T."s)));
  EXPECT_EQ(WILDCARD, get_color(short_string_to_color("F."s)));
  EXPECT_EQ(BACKREF, get_color(short_string_to_color("TB"s)));
  EXPECT_EQ(BACKREF, get_color(short_string_to_color("FB"s)));
  EXPECT_EQ(NEXT_CUSTOM, get_color(short_string_to_color("TU"s)));
  EXPECT_EQ(NEXT_CUSTOM, get_color(short_string_to_color("FU"s)));
  EXPECT_EQ(Color(+NEXT_CUSTOM + 1), get_color(short_string_to_color("TV"s)));
  EXPECT_EQ(Color(+NEXT_CUSTOM + 1), get_color(short_string_to_color("FV"s)));
  EXPECT_EQ(Color(+NEXT_CUSTOM + 2), get_color(short_string_to_color("TW"s)));
  EXPECT_EQ(Color(+NEXT_CUSTOM + 2), get_color(short_string_to_color("FW"s)));

  using enum RuleSide;
  EXPECT_EQ(TO, get_rule_side(short_string_to_color("TR"s)));
  EXPECT_EQ(FROM, get_rule_side(short_string_to_color("FR"s)));
  EXPECT_EQ(TO, get_rule_side(short_string_to_color("T."s)));
  EXPECT_EQ(FROM, get_rule_side(short_string_to_color("F."s)));
  EXPECT_EQ(TO, get_rule_side(short_string_to_color("TB"s)));
  EXPECT_EQ(FROM, get_rule_side(short_string_to_color("FB"s)));
  EXPECT_EQ(TO, get_rule_side(short_string_to_color("TU"s)));
  EXPECT_EQ(FROM, get_rule_side(short_string_to_color("FU"s)));
  EXPECT_EQ(TO, get_rule_side(short_string_to_color("TV"s)));
  EXPECT_EQ(FROM, get_rule_side(short_string_to_color("FV"s)));
  EXPECT_EQ(TO, get_rule_side(short_string_to_color("TW"s)));
  EXPECT_EQ(FROM, get_rule_side(short_string_to_color("FW"s)));
}

} // namespace color::test
