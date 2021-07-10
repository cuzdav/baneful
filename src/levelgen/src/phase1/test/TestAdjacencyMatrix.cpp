#include "AdjacencyMatrix.hpp"
#include "gtest/gtest.h"

TEST(AdjacencyMatrix, basic_edges) {
  p1::AdjacencyMatrix am(3);
  // 0---->1--\
  //  \________2
  am.add_edge(0, 1);
  am.add_edge(0, 2);
  am.add_edge(1, 2);

  EXPECT_EQ(0, am.indegree_of(0));
  EXPECT_EQ(1, am.indegree_of(1));
  EXPECT_EQ(2, am.indegree_of(2));

  EXPECT_EQ(0, am.outdegree_of(2));
  EXPECT_EQ(1, am.outdegree_of(1));
  EXPECT_EQ(2, am.outdegree_of(0));

  EXPECT_TRUE(am.has_edge(0, 1));
  EXPECT_TRUE(am.has_edge(0, 2));
  EXPECT_TRUE(am.has_edge(1, 2));

  EXPECT_FALSE(am.has_edge(0, 0));
  EXPECT_FALSE(am.has_edge(1, 0));
  EXPECT_FALSE(am.has_edge(1, 1));
  EXPECT_FALSE(am.has_edge(2, 0));
  EXPECT_FALSE(am.has_edge(2, 1));
  EXPECT_FALSE(am.has_edge(2, 2));
}
