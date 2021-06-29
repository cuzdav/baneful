#include <string_view>
#include <gtest/gtest.h>

#include "Color.hpp"
#include "Verticies.hpp"
#include "Transforms.hpp"

using namespace p1;
using namespace boost;
using namespace std::literals;

json::object make_rotating_color_transform(std::string const& cycle_colors) {
  json::object transform;
  transform["type"] = "RotatingColors";
  transform["cycle_chars"] = cycle_colors;
  return transform;
}

TEST(TestTransforms, BasicInterface) {
  Transforms t;
  Verticies v;

  t.add_level_type_override('a', make_rotating_color_transform("bc"));
  int index = t.add_vertex("ab", RuleSide::TO, v);


}
