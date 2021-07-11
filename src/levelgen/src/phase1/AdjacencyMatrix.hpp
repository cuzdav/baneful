#pragma once

#include <numeric>
#include <cassert>
#include <vector>

class AdjacencyMatrix {
public:
  AdjacencyMatrix(int num_vertices)
      : num_vertices_(num_vertices),
        adjacency_matrix_(num_vertices * num_vertices) {
  }

  void
  add_edge(int from, int to) {
    int idx                = to_flat_idx(from, to);
    adjacency_matrix_[idx] = true;
  }

  void
  remove_edge(int from, int to) {
    int idx                = to_flat_idx(from, to);
    adjacency_matrix_[idx] = false;
  }

  int
  size() const {
    return num_vertices_;
  }

  bool
  has_edge(int from, int to) const {
    return adjacency_matrix_[to_flat_idx(from, to)];
  }

  int
  outdegree_of(int idx) const {
    auto b = adjacency_matrix_.begin() + idx * num_vertices_;
    return std::accumulate(b, b + num_vertices_, 0);
  }

  int
  indegree_of(int idx) const {
    int inedges = 0;
    for (int row = 0; row < num_vertices_; row++) {
      inedges += adjacency_matrix_[row * num_vertices_ + idx];
    }
    return inedges;
  }

  // for given vertex index, make a callback providing each index of source
  // vertex with an incoming edge.
  // return: number of incoming edges (indegree)
  template <typename CallbackT>
  int
  visit_parents_of(int idx, CallbackT callback) {
    int inedges = 0;
    for (int row = 0; row < num_vertices_; row++) {
      if (adjacency_matrix_[row * num_vertices_ + idx]) {
        ++inedges;
        callback(row);
      }
    }
    return inedges;
  }

  // for given vertex index, make a callback providing each index of child
  // vertex, connected by an outgoing edge.
  // return: number of outgoing edges (outdegree)
  template <typename CallbackT>
  int
  visit_children_of(int idx, CallbackT callback) {
    int outedges = 0;
    for (int col = 0; col < num_vertices_; col++) {
      if (adjacency_matrix_[idx * num_vertices_ + col]) {
        ++outedges;
        callback(col);
      }
    }
    return outedges;
  }

private:
  int
  to_flat_idx(int from, int to) const {
    auto idx = num_vertices_ * from + to;
    assert(idx < adjacency_matrix_.size());
    return idx;
  }

private:
  int num_vertices_;

  // Minimizing allocations is desirable.  This gives 64 edges in 8 bytes.
  // TODO: make a small SBO version?
  std::vector<bool> adjacency_matrix_;
};
