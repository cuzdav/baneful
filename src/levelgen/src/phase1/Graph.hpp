#pragma once
#include "Vertex.hpp"
#include "boost/json.hpp"

#include <vector>

namespace p1::vertex {
enum class Vertex : std::uint32_t;
}

namespace p1 {

class Graph {
  using VertexVec = std::vector<vertex::Vertex>;

public:
  Graph(boost::json::object const & level);

private:
  VertexVec         nodes_;
  std::vector<bool> adjacency_matrix;
};

} // namespace p1
