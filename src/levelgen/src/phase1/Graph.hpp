#pragma once
#include "AdjacencyMatrix.hpp"
#include "Vertices.hpp"

class Graph {

public:
  Graph(Vertices && vertices, matrix::AdjacencyMatrix && adjacency_matrix)
      : vertices_(vertices), adjacency_matrix_(adjacency_matrix) {
  }

private:
  Vertices                vertices_;
  matrix::AdjacencyMatrix adjacency_matrix_;
};
