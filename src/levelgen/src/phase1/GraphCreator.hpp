#pragma once
#include "AdjacencyMatrix.hpp"
#include "RuleSide.hpp"
#include "Transforms.hpp"
#include "Vertex.hpp"
#include "Vertices.hpp"

#include <boost/json.hpp>
#include <optional>
#include <vector>

namespace vertex {
enum class Vertex : std::uint32_t;
}

class GraphCreator {
  using VertexVec = std::vector<vertex::Vertex>;

public:
  using const_iterator = typename VertexVec::const_iterator;

  GraphCreator(boost::json::object const & level);

  //  Graph create();

  Vertices const &
  get_vertices() const {
    return vertices_;
  }

  Vertices &
  get_vertices() {
    return vertices_;
  }

  Transforms const &
  get_transforms() const {
    return transforms_;
  }

  Transforms &
  get_transforms() {
    return transforms_;
  }

  AdjacencyMatrix const &
  get_adjacency_matrix() const {
    return *adjacency_matrix_;
  }

  bool
  has_edge(int from_idx, int to_idx) {
    return adjacency_matrix_->has_edge(from_idx, to_idx);
  }

  // when two vertices are adjacent, and have the same color, and there is only
  // one edge into the given "target" vertex, and there is room in the "from"
  // vertex of that edge, then merge them together into the same node, making it
  // a multi-block vertex.  (The graph cannot be created merged, because
  // we don't know where the edges until its construction has completed.)
  void compress_vertices();

private:
  void add_rules(boost::json::object const & rules);

  enum class TraverseAction { CREATE_VERTEX_ONLY, CREATE_EDGES };
  void traverse_input(boost::json::object const & rules, TraverseAction action);

  // return idx of last vertex in chain
  int process_chain(boost::json::string_view chain, RuleSide side, int prev_idx,
                    TraverseAction action);

  // while compressing vertices, we found two elgible vertices to merge. Do so
  // if possible.

  bool try_to_merge(int from_idx, int to_idx);
  void remove_vertex(int doomed_idx, int parent_idx);
  void give_vertex_out_edges_to_parent(int vertex_idx, int parent_idx);
  void vertex_moved(int old_idx, int new_idx);

private:
  Transforms                     transforms_;
  Vertices                       vertices_;
  VertexVec                      nodes_;
  std::optional<AdjacencyMatrix> adjacency_matrix_;
};
