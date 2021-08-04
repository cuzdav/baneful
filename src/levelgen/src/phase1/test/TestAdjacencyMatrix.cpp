#include "AdjacencyMatrix.hpp"
#include "gtest/gtest.h"

TEST(TestAdjacencyMatrix, basic_edges) {
  AdjacencyMatrix am(3);
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

TEST(TestAdjacencyMatrix, visit_parents) {
  AdjacencyMatrix am(4);
  // 0---->1--\----\
  //  \________2----\
  //   \_____________3
  //
  am.add_edge(0, 1);
  am.add_edge(0, 2);
  am.add_edge(0, 3);
  am.add_edge(1, 2);
  am.add_edge(1, 3);
  am.add_edge(2, 3);

  int indegree   = 0;
  int edge_count = 0;
  int edges[4];

  auto accept_edge = [&](int idx) { edges[edge_count++] = idx; };

  indegree = am.visit_parents_of(0, accept_edge);
  EXPECT_EQ(0, indegree);
  EXPECT_EQ(0, edge_count);

  indegree = am.visit_parents_of(1, accept_edge);
  EXPECT_EQ(1, indegree);
  EXPECT_EQ(1, edge_count);
  EXPECT_EQ(0, edges[0]);

  indegree   = 0;
  edge_count = 0;
  indegree   = am.visit_parents_of(2, accept_edge);
  EXPECT_EQ(2, indegree);
  EXPECT_EQ(2, edge_count);
  EXPECT_EQ(0, edges[0]);
  EXPECT_EQ(1, edges[1]);

  indegree   = 0;
  edge_count = 0;
  indegree   = am.visit_parents_of(3, accept_edge);
  EXPECT_EQ(3, indegree);
  EXPECT_EQ(3, edge_count);
  EXPECT_EQ(0, edges[0]);
  EXPECT_EQ(1, edges[1]);
  EXPECT_EQ(2, edges[2]);
}

TEST(TestAdjacencyMatrix, visit_children) {
  AdjacencyMatrix am(4);
  // 0---->1--\----\
  //  \________2----\
  //   \_____________3
  //
  am.add_edge(0, 1);
  am.add_edge(0, 2);
  am.add_edge(0, 3);
  am.add_edge(1, 2);
  am.add_edge(1, 3);
  am.add_edge(2, 3);

  int outdegree  = 0;
  int edge_count = 0;
  int edges[4];

  auto accept_edge = [&](int idx) { edges[edge_count++] = idx; };

  outdegree = am.visit_children_of(0, accept_edge);
  EXPECT_EQ(3, outdegree);
  EXPECT_EQ(3, edge_count);
  EXPECT_EQ(1, edges[0]);
  EXPECT_EQ(2, edges[1]);
  EXPECT_EQ(3, edges[2]);

  outdegree  = 0;
  edge_count = 0;
  outdegree  = am.visit_children_of(1, accept_edge);
  EXPECT_EQ(2, outdegree);
  EXPECT_EQ(2, edge_count);
  EXPECT_EQ(2, edges[0]);
  EXPECT_EQ(3, edges[1]);

  outdegree  = 0;
  edge_count = 0;
  outdegree  = am.visit_children_of(2, accept_edge);
  EXPECT_EQ(1, outdegree);
  EXPECT_EQ(1, edge_count);
  EXPECT_EQ(3, edges[0]);

  outdegree  = 0;
  edge_count = 0;
  outdegree  = am.visit_children_of(3, accept_edge);
  EXPECT_EQ(0, outdegree);
  EXPECT_EQ(0, edge_count);
}

TEST(TestAdjacencyMatrix, test_swap_rows) {
  AdjacencyMatrix am(4);
  // 0---->1--\----\
  //  \________2----\
  //   \_____________3
  //
  am.add_edge(0, 1);
  am.add_edge(0, 2);
  am.add_edge(0, 3);
  am.add_edge(1, 2);
  am.add_edge(1, 3);
  am.add_edge(2, 3);

  EXPECT_EQ(3, am.outdegree_of(0));
  EXPECT_EQ(2, am.outdegree_of(1));
  EXPECT_EQ(1, am.outdegree_of(2));
  EXPECT_EQ(0, am.outdegree_of(3));

  EXPECT_EQ(0, am.indegree_of(0));
  EXPECT_EQ(1, am.indegree_of(1));
  EXPECT_EQ(2, am.indegree_of(2));
  EXPECT_EQ(3, am.indegree_of(3));

  EXPECT_TRUE(am.has_edge(0, 1));
  EXPECT_TRUE(am.has_edge(0, 2));
  EXPECT_TRUE(am.has_edge(0, 3));
  EXPECT_TRUE(am.has_edge(1, 2));
  EXPECT_TRUE(am.has_edge(1, 3));
  EXPECT_TRUE(am.has_edge(2, 3));
  EXPECT_FALSE(am.has_edge(3, 0));
  EXPECT_FALSE(am.has_edge(3, 1));
  EXPECT_FALSE(am.has_edge(3, 2));
  EXPECT_FALSE(am.has_edge(2, 1));
  EXPECT_FALSE(am.has_edge(2, 0));
  EXPECT_FALSE(am.has_edge(1, 0));

  am.debug_dump("Before");

  am.swap_rows(0, 3);

  am.debug_dump("After ");

  EXPECT_EQ(0, am.outdegree_of(0));
  EXPECT_EQ(2, am.outdegree_of(1));
  EXPECT_EQ(1, am.outdegree_of(2));
  EXPECT_EQ(3, am.outdegree_of(3));

  EXPECT_EQ(3, am.indegree_of(0));
  EXPECT_EQ(1, am.indegree_of(1));
  EXPECT_EQ(2, am.indegree_of(2));
  EXPECT_EQ(0, am.indegree_of(3));

  EXPECT_FALSE(am.has_edge(0, 1));
  EXPECT_FALSE(am.has_edge(0, 2));
  EXPECT_FALSE(am.has_edge(0, 3));
  EXPECT_TRUE(am.has_edge(1, 2));
  EXPECT_FALSE(am.has_edge(1, 3));
  EXPECT_FALSE(am.has_edge(2, 3));
  EXPECT_TRUE(am.has_edge(3, 0));
  EXPECT_TRUE(am.has_edge(3, 1));
  EXPECT_TRUE(am.has_edge(3, 2));
  EXPECT_FALSE(am.has_edge(2, 1));
  EXPECT_TRUE(am.has_edge(2, 0));
  EXPECT_TRUE(am.has_edge(1, 0));
}
