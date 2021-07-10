#pragma once
#include "AdjacencyMatrix.hpp"
#include "RuleSide.hpp"
#include "Transforms.hpp"
#include "Vertex.hpp"
#include "Verticies.hpp"

#include <boost/json.hpp>
#include <optional>
#include <vector>

namespace p1::vertex {
enum class Vertex : std::uint32_t;
}

namespace p1 {

class GraphCreator {
  using VertexVec = std::vector<vertex::Vertex>;

public:
  using const_iterator = typename VertexVec::const_iterator;

  GraphCreator(boost::json::object const & level);

  Verticies const &
  get_verticies() const {
    return verticies_;
  }

  Transforms const &
  get_transforms() const {
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

  // when two verticies are adjacent, and have the same color, and there is only
  // one edge into the given "target" vertex, and there is room in the "from"
  // vertex of that edge, then merge them together into the same node, making it
  // a multi-block vertex.  (The graph cannot be created merged, because
  // we don't know where the edges until its construction has completed.)
  void merge_like_verticies();

private:
  void add_rules(boost::json::object const & rules);

  enum class TraverseAction { CREATE_VERTEX_ONLY, CREATE_EDGES };
  void traverse_input(boost::json::object const & rules, TraverseAction action);

  // return idx of last vertex in chain
  int process_chain(boost::json::string_view chain, RuleSide side, int prev_idx,
                    TraverseAction action);

private:
  Transforms                     transforms_;
  Verticies                      verticies_;
  VertexVec                      nodes_;
  std::optional<AdjacencyMatrix> adjacency_matrix_;
};

} // namespace p1
