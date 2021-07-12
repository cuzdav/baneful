#pragma once
#include "GraphCreator.hpp"
#include <string>

namespace utils {

inline std::string
visit_children_starting_at(int idx, GraphCreator const & graph_creator) {
  auto const & am     = graph_creator.get_adjacency_matrix();
  auto const & xforms = graph_creator.get_transforms();
  auto         vertex = graph_creator.get_vertices()[idx];
  std::string  result = to_external_short_name(vertex, xforms);
  if (am.outdegree_of(idx) > 0) {
    am.visit_children_of(idx, [&](int child) {
      auto rest = visit_children_starting_at(child, graph_creator);
      if (not rest.empty()) {
        result += "->" + rest;
      }
    });
  }
  return result;
}

inline std::string
graph_to_string(GraphCreator const & graph_creator) {
  std::string result;
  auto &      am = graph_creator.get_adjacency_matrix();
  am.visit_start_vertices([&](int start) {
    result += to_external_short_name(graph_creator.get_vertices()[start],
                                     graph_creator.get_transforms());
    am.visit_children_of(start, [&](int child) {
      auto rest = visit_children_starting_at(child, graph_creator) + "\n";
      if (not rest.empty()) {
        result += "->" + rest;
      }
    });
  });
  return result;
}

} // namespace utils
