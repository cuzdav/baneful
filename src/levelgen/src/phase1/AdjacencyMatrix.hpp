#pragma once

#include <numeric>
#include <cassert>
#include <vector>

namespace matrix {

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
    auto b        = adjacency_matrix_.begin() + idx * num_vertices_;
    auto outedges = std::accumulate(b, b + num_vertices_, 0);
    return outedges;
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
  visit_parents_of(int idx, CallbackT callback) const {
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
  visit_children_of(int idx, CallbackT callback) const {
    int outedges = 0;
    for (int col = 0; col < num_vertices_; col++) {
      if (adjacency_matrix_[idx * num_vertices_ + col]) {
        ++outedges;
        callback(col);
      }
    }
    return outedges;
  }

  template <typename CallbackT>
  void
  visit_start_vertices(CallbackT visitor, bool process_isolated = false) const {
    for (int i = 0; i < num_vertices_; ++i) {
      if (indegree_of(i) == 0 && (process_isolated || outdegree_of(i) > 0)) {
        visitor(i);
      }
    }
  }

  void
  swap_rows(int idx1, int idx2) {
    if (idx1 == idx2) {
      return;
    }
    // swap rows idx1 and idx2
    for (int i = 0; i < num_vertices_; ++i) {
      std::swap(adjacency_matrix_[to_flat_idx(idx1, i)],
                adjacency_matrix_[to_flat_idx(idx2, i)]);
    }

    // swap columns idx1 and idx2
    for (int i = 0; i < num_vertices_; ++i) {
      std::swap(adjacency_matrix_[to_flat_idx(i, idx1)],
                adjacency_matrix_[to_flat_idx(i, idx2)]);
    }
  }

  void
  resize_down(int num_vertices) {
    // verify
    assert(num_vertices < num_vertices_);
    for (int i = num_vertices; i < num_vertices_; ++i) {
      for (int j = 0; j < num_vertices_; ++j) {
        assert(adjacency_matrix_[to_flat_idx(i, j)] == false);
        assert(adjacency_matrix_[to_flat_idx(j, i)] == false);
      }
    }

    // perform (copy down into smaller square)
    for (int i = 0; i < num_vertices; ++i) {
      for (int j = 0; j < num_vertices; ++j) {
        adjacency_matrix_[num_vertices * i + j] =
            adjacency_matrix_[num_vertices_ * i + j];
      }
    }
    adjacency_matrix_.resize(num_vertices * num_vertices);
    num_vertices_ = num_vertices;
  }

  std::vector<bool> const &
  adjacency_matrix() const {
    return adjacency_matrix_;
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
  // TODO: make a small SBO version that doesn't usually allocate anything?
  std::vector<bool> adjacency_matrix_;
};

} // namespace matrix
