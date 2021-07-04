#pragma once
#include "Transforms.hpp"
#include "Vertex.hpp"

#include <boost/json.hpp>
#include <string>
#include <vector>

namespace p1::vertex {
  enum class Vertex : std::uint32_t;
}

namespace p1 {

  class Transforms;
  class Verticies;

  class Graph {
    using VertexVec = std::vector<vertex::Vertex>;
  public:

    using const_iterator = typename VertexVec::const_iterator;

    Graph(boost::json::object const & level);

    const_iterator begin() const { return nodes_.begin(); }
    const_iterator end() const { return nodes_.end(); }

  private:
    void add_rules(boost::json::object const & rules,
                   Transforms & transforms,
                   Verticies & verticies);

  private:
    VertexVec nodes_;
    std::vector<bool> adjacency_matrix;
  };

} // p1
