#include "AdjacencyMatrix.hpp"

#include <iostream>

void
AdjacencyMatrix::debug_dump(char const * msg) const {
  std::cout << msg << "\n";
  int col = 0;
  int row = 0;
  for (int i = 0; i < num_vertices_; ++i) {
    std::cout << "\t" << i;
  }
  std::cout << std::endl;
  for (bool has_edge : adjacency_matrix_) {
    if (col++ == 0) {
      std::cout << row++;
    }
    std::cout << "\t" << has_edge;
    if (col == num_vertices_) {
      col = 0;
      std::cout << "\n";
    }
  }
}
