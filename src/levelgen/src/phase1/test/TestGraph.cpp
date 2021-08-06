#include "Color.hpp"
#include "Graph.hpp"
#include "GraphCreator.hpp"
#include "jsonutil.hpp"
#include "Vertex.hpp"
#include "Vertices.hpp"

#include "jsonlevelconfig.hpp"

#include <gtest/gtest.h>
#include <boost/json.hpp>

using namespace test::json;

TEST(TestGraph, test_simple_isoporphism) {
  auto lvl1 = level(rules(from("a") = to("b")));
  auto lvl2 = level(rules(from("a") = to("b")));

  Graph g1 = GraphCreator(lvl1).create();
  Graph g2 = GraphCreator(lvl2).create();

  EXPECT_TRUE(g1.check_isomorphism(g2));
}
