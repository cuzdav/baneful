#pragma once

#include "Color.hpp"

namespace color::test {

using enum Color;
using enum RuleSide;
using namespace std::literals;

namespace fc {
constexpr auto noth_to = to_final_color(NOTHING, TO);
// there is no noth_fm
constexpr auto rect_to  = to_final_color(SOLID_RECTANGLE, TO);
constexpr auto rect_fm  = to_final_color(SOLID_RECTANGLE, FROM);
constexpr auto wild_to  = to_final_color(WILDCARD, TO);
constexpr auto wild_fm  = to_final_color(WILDCARD, FROM);
constexpr auto bref_to  = to_final_color(BACKREF, TO);
constexpr auto bref_fm  = to_final_color(BACKREF, FROM);
constexpr auto rotc_to  = to_final_color(ROTATING_COLORS, TO);
constexpr auto rotc_fm  = to_final_color(ROTATING_COLORS, FROM);
constexpr auto cust_to  = to_final_color(NEXT_CUSTOM, TO);
constexpr auto cust_fm  = to_final_color(NEXT_CUSTOM, FROM);
constexpr auto cust2_to = to_final_color(Color(+NEXT_CUSTOM + 1), TO);
constexpr auto cust2_fm = to_final_color(Color(+NEXT_CUSTOM + 1), FROM);
constexpr auto cust3_to = to_final_color(Color(+NEXT_CUSTOM + 2), TO);
constexpr auto cust3_fm = to_final_color(Color(+NEXT_CUSTOM + 2), FROM);
} // namespace fc

namespace short_string {
// std::string not yet constexpr (gcc 11)
inline auto noth_to  = to_short_string(fc::noth_to);
inline auto rect_to  = to_short_string(fc::rect_to);
inline auto rect_fm  = to_short_string(fc::rect_fm);
inline auto wild_to  = to_short_string(fc::wild_to);
inline auto wild_fm  = to_short_string(fc::wild_fm);
inline auto bref_to  = to_short_string(fc::bref_to);
inline auto bref_fm  = to_short_string(fc::bref_fm);
inline auto rotc_to  = to_short_string(fc::rotc_to);
inline auto rotc_fm  = to_short_string(fc::rotc_fm);
inline auto cust_to  = to_short_string(fc::cust_to);
inline auto cust_fm  = to_short_string(fc::cust_fm);
inline auto cust2_to = to_short_string(fc::cust2_to);
inline auto cust2_fm = to_short_string(fc::cust2_fm);
inline auto cust3_to = to_short_string(fc::cust3_to);
inline auto cust3_fm = to_short_string(fc::cust3_fm);
} // namespace short_string

} // namespace color::test
