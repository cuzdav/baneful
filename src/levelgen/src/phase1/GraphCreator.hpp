#pragma once
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

    Verticies const& get_verticies() const { return verticies_; }
    Transforms const& get_transforms() const { return transforms_; }

    void add_rules(boost::json::object const & rules);

  private:
    Transforms transforms_;
    Verticies verticies_;
    VertexVec nodes_;
    std::vector<bool> adjacency_matrix;
  };

} // p1
