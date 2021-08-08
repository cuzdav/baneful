#pragma once
#include "AdjacencyMatrix.hpp"
#include "Color.hpp"
#include "Vertices.hpp"
#include "Vertex.hpp"

#include <array>
#include <algorithm>
#include <vector>

class Graph {

public:
  using Vertex              = vertex::Vertex;
  using Index               = int;
  using IndexVec            = std::vector<Index>;
  using IndexRange          = std::pair<Index, Index>;
  using IndexRangeVec       = std::vector<IndexRange>;
  using AdjacencyMatrix     = matrix::AdjacencyMatrix;
  using Block               = block::FinalBlock;
  using BlockEquivalenceMap = Block[16];

  Graph(Vertices && vertices, matrix::AdjacencyMatrix && adjacency_matrix,
        std::string level_name = "unspecified")
      : level_name_(std::move(level_name)),
        adjacency_matrix_(adjacency_matrix),
        vertices_(vertices),
        indices_(vertices.size()) {
    populate_colorgroups();
  }

  bool check_isomorphism(Graph const & other) const;

  IndexRangeVec const &
  permutable_block_ranges() const {
    return permutable_block_ranges_;
  }

  Vertices const &
  vertices() const {
    return vertices_;
  };

  void dump(char const *) const;

  std::string const &
  level_name() const {
    return level_name_;
  }

  Index
  mapped_index(Index idx) const {
    return indices_[idx];
  }

  AdjacencyMatrix const &
  adjacency_matrix() const {
    return adjacency_matrix_;
  }

private:
  void populate_colorgroups();
  void append_colorgroup(Index from, Index to);
  bool equivalent_adjacency_matricies(Graph const & other) const;

private:
  std::string     level_name_;
  IndexRangeVec   permutable_block_ranges_;
  AdjacencyMatrix adjacency_matrix_;
  Vertices        vertices_;

  // Optimization: could be declared in check_isomorphism but that would require
  // reallocating it every call.
  mutable IndexVec indices_;
};

// visit N simultaneous graphs, receiving N permutable range begin/end pairs in
// each callback. If graphs have unequal numbers of ranges, stops after the
// first one runs out. Returns true if all were visited, or false if one (or
// more) were skipped, due to one graph running out first.
template <typename CallbackT, typename... GraphT>
bool
visit_permutable_ranges(CallbackT cb, GraphT const &... graphs) {
  std::array<std::size_t, sizeof...(GraphT)> sizes{
      size(graphs.permutable_block_ranges())...};
  auto [minsize, maxsize] = std::minmax(begin(sizes), end(sizes));

  for (size_t idx = 0; idx != minsize; ++idx) {
    cb((graphs.permutable_block_ranges()[idx])...);
  }
  return minsize == maxsize;
}

// Takes N graphs, makes callbacks passing N corresponding vertices from each
// graph to callback, each outside the permutable block ranges.
//
// PREREQ: all graphs have the same permutable block ranges and number of
// indices.
template <typename CallbackT, typename... GraphT>
void
visit_nonpermutable_vertices(CallbackT cb, Graph const & graph1,
                             GraphT const &... graphs) {
  Graph::Index idx = 0;
  for (auto [begin_idx, end_idx] : graph1.permutable_block_ranges()) {
    while (idx != begin_idx) {
      cb(graph1.vertices().values()[idx], graphs.vertices().values()[idx]...);
      idx++;
    }
    // jump to start of next nonpermutable range
    idx = end_idx;
  }
  // Visit the rest...
  auto end_idx = graph1.vertices().size();
  while (idx != end_idx) {
    cb(graph1.vertices().values()[idx], graphs.vertices().values()[idx]...);
    idx++;
  }
}
