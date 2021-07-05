#include "GraphCreator.hpp"
#include "jsonlevelconfig.hpp"
#include "jsonutil.hpp"

#include "boost/json.hpp"
#include "gtest/gtest.h"

namespace p1::test {

using namespace std::literals;

TEST(TestGraphCreator, simple_rule1) {

  auto lvl = level(rules(from("a") = to("")));

  GraphCreator gc(lvl);
  Verticies const & verticies = gc.get_verticies();

  std::vector<std::string> names{verticies.names_begin(), verticies.names_end()};
  ASSERT_NE(names.begin(), names.end());

  // FROM:RECT[a], TO:NOTHING[ ]
  ASSERT_EQ((std::vector<std::string>{"%a", "\" "}), names);

  // for (vertex::Vertex vertex : graph) {
  //   std::cout << to_string(vertex) << std::endl;
  // }
}

} // namespace p1::test
