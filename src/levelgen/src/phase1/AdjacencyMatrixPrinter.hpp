#pragma once

#include "AdjacencyMatrix.hpp"
#include "Vertices.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <fmt/format.h>

namespace matrix {

template <typename LabelerT>
std::string
to_string(AdjacencyMatrix const & adjmtx, LabelerT && labeler) {
  int col = 0;
  int row = 0;

  int width = 0;
  for (int i = 0; i < adjmtx.size(); ++i) {
    auto label = labeler(i);
    if (size(label) > width) {
      width = size(label);
    }
  }
  width += 1 + (3 + (adjmtx.size() > 9));

  std::string result(width, ' ');
  for (int i = 0; i < adjmtx.size(); ++i) {
    auto label = fmt::format("{}({:d})", labeler(i), i);
    result += fmt::format("{:^{}}", label, width);
  }
  result += '\n';
  for (bool has_edge : adjmtx.adjacency_matrix()) {
    if (col++ == 0) {
      auto label = fmt::format("{}({:d})", labeler(row), row);
      result += fmt::format("{:<{}}", label, width);
      row++;
    }
    result += fmt::format("{:^{}d}", has_edge, width);
    if (col == adjmtx.size()) {
      col = 0;
      result += "\n";
    }
  }
  return result;
}

inline std::string
to_string(AdjacencyMatrix const & adjmtx) {
  return to_string(adjmtx, [](int i) { return std::to_string(i); });
}

inline std::ostream &
operator<<(std::ostream & os, AdjacencyMatrix const & adjmtx) {
  return os << to_string(adjmtx);
}

// NAMES ONLY

struct WithNames {
  AdjacencyMatrix const &          matrix_;
  std::vector<std::string> const & names_;
};

inline std::string
to_string(WithNames const & adjmtx_wrapper) {
  return to_string(adjmtx_wrapper.matrix_,
                   [&](int i) { return adjmtx_wrapper.names_.at(i); });
}

inline std::ostream &
operator<<(std::ostream & os, WithNames const & adjmtx) {
  return os << to_string(adjmtx);
}

// VERTICES: uses names but appends extra information only knowable from the
// vertices themselves (combines names vector with graph info ):
// * start bit indicated with leading ^
// * colon separates the blocks in "this" vertex from those that follow

struct WithVertices {
  AdjacencyMatrix const & matrix_;
  Vertices const &        vertices_;
};

inline std::string
to_string(WithVertices const & adjmtx_wrapper) {
  return to_string(adjmtx_wrapper.matrix_, [&](int i) {
    return adjmtx_wrapper.vertices_.pretty_name(i);
  });
}

inline std::ostream &
operator<<(std::ostream & os, WithVertices const & adjmtx) {
  return os << to_string(adjmtx);
}

} // namespace matrix
