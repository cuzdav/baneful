#include "Color.hpp"
#include "Graph.hpp"
#include "jsonutil.hpp"
#include "Vertex.hpp"
#include "Verticies.hpp"

#include "jsonlevelconfig.hpp"

#include <gtest/gtest.h>
#include <boost/json.hpp>

#include <initializer_list>
#include <string_view>

namespace p1::test {

TEST(TestGraph, BasicInterface) {

  auto lvl = level(
    rules(
      from("abc") = to("a", "b")
    ),
    type_overrides(
      std::pair{"a", rotating_colors("bcd")}
    )
  );

  jsonutil::pretty_print(std::cout, lvl);

  Graph graph(lvl);

  // ASSERT_NE(graph.begin(), graph.end());

  // for (vertex::Vertex vertex : graph) {
  //   std::cout << to_string(vertex) << std::endl;
  // }

}

} // p1::test
