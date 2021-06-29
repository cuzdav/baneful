#pragma once
#include "Transforms.hpp"
#include "Vertex.hpp"

#include <boost/json.hpp>
#include <string>

namespace p1 {

  class Transforms;
  class Verticies;
  enum class Vertex : std::uint32_t;

  class Graph {
  public:
    Graph(boost::json::object const & level);

  private:
    void add_rules(boost::json::object const & rules,
                   Transforms & transforms,
                   Verticies & verticies);

  private:
    enum class Vertex : std::uint32_t;
    std::vector<Vertex> nodex_;
    std::vector<bool> adjacency_matrix;
  };

} // p1
