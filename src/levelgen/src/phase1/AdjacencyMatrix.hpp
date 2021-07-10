#pragma once

#include <numeric>
#include <cassert>
#include <vector>

namespace p1 {

class AdjacencyMatrix {
public:
  AdjacencyMatrix(int num_verticies)
      : num_verticies_(num_verticies),
        adjacency_matrix_(num_verticies * num_verticies) {
  }

  void
  add_edge(int from, int to) {
    int idx                = to_flat_idx(from, to);
    adjacency_matrix_[idx] = true;
  }

  int
  size() const {
    return num_verticies_;
  }

  bool
  has_edge(int from, int to) const {
    return adjacency_matrix_[to_flat_idx(from, to)];
  }

  int
  outdegree_of(int idx) const {
    auto b = adjacency_matrix_.begin() + idx * num_verticies_;
    return std::accumulate(b, b + num_verticies_, 0);
  }

  int
  indegree_of(int idx) const {
    int inedges = 0;
    for (int row = 0; row < num_verticies_; row++) {
      inedges += adjacency_matrix_[row * num_verticies_ + idx];
    }
    return inedges;
  }

private:
  int
  to_flat_idx(int from, int to) const {
    auto idx = num_verticies_ * from + to;
    assert(idx < adjacency_matrix_.size());
    return idx;
  }

private:
  int               num_verticies_;
  std::vector<bool> adjacency_matrix_; // vector bool?  Really?  Yep.
};

} // namespace p1
