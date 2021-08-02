#pragma once
#include "AdjacencyMatrix.hpp"
#include "Vertices.hpp"

class Graph {

public:
  Graph(Vertices && vertices, AdjacencyMatrix && adjacency_matrix)
      : vertices_(vertices), adjacency_matrix_(adjacency_matrix) {
  }

private:
  Vertices        vertices_;
  AdjacencyMatrix adjacency_matrix_;
};
