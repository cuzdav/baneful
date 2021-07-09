#pragma once
#include "RuleSide.hpp"
#include "Transforms.hpp"
#include "Vertex.hpp"
#include "Verticies.hpp"

#include <boost/json.hpp>
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

  void add_rules(boost::json::object const & rules);

  bool has_edge(int from, int to);

private:
  enum class TraverseAction { CREATE_VERTEX_ONLY, CREATE_EDGES };
  void traverse_input(boost::json::object const & rules, TraverseAction action);

  // return idx of last vertex in chain
  int process_chain(boost::json::string_view chain, RuleSide side, int prev_idx,
                    TraverseAction action);

  void add_edge(int from, int to);

  int to_flat_idx(int from, int to);

private:
  Transforms        transforms_;
  Verticies         verticies_;
  VertexVec         nodes_;
  int               adj_row_width_ = 0;
  std::vector<bool> adjacency_matrix_;
};

} // namespace p1
