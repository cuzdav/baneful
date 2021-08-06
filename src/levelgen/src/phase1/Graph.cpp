#include "Block.hpp"
#include "Color.hpp"
#include "Graph.hpp"
#include "AdjacencyMatrixPrinter.hpp"

#include <algorithm>
#include <iostream>

void
Graph::append_colorgroup(Graph::Index from, Graph::Index to) {
  if (to - from > 1) {
    permutable_block_ranges_.push_back({from, to});
  }
}

void
Graph::populate_colorgroups() {
  color::FinalColor color{};
  Graph::Index      from = 0;
  Graph::Index      to   = 0;
  for (auto vertex : vertices_.values()) {
    if (auto cur_color = get_final_color(vertex); cur_color != color) {
      append_colorgroup(from, to);
      from  = to;
      color = cur_color;
    }
    ++to;
  }
  append_colorgroup(from, to);
}

static bool
is_valid_mapping(Graph::BlockEquivalenceMap & map, Graph::Block b1,
                 Graph::Block b2) {
  if (map[+b1] == block::Unused) {
    map[+b1] = b2;
  }
  return map[+b1] == b2;
}

static bool
check_blocks(Graph::BlockEquivalenceMap & blockmap, Graph::Vertex v1,
             Graph::Vertex v2) {
  assert(get_final_color(v1) == get_final_color(v2));

  auto const sz1 = size(v1);
  if (sz1 != size(v2)) {
    return false;
  }

  if (has_dynamic_block_colors(get_final_color(v1))) {
    for (int i = 0; i < sz1; ++i) {
      if (not is_valid_mapping(blockmap, get_block(v1, i), get_block(v2, i))) {
        return false;
      }
    }
  }
  else {
    for (int i = 0; i < sz1; ++i) {
      if (get_block(v1, i) != get_block(v2, i)) {
        return false;
      }
    }
  }
  return true;
}

bool
check_basic_colorgroup_compatibility(Graph const & graph1, Graph const & graph2,
                                     Graph::BlockEquivalenceMap & colormap) {
  if (not graph1.vertices().compatible_number_and_colors(graph2.vertices())) {
    return false;
  }

  bool valid = true;
  visit_nonpermutable_vertices(
      [&](vertex::Vertex v1, vertex::Vertex v2) {
        valid &= check_blocks(colormap, v1, v2);
      },
      graph1,
      graph2);

  return valid;
}

bool
Graph::check_dynamic_equivalence(Graph::BlockEquivalenceMap & colormap,
                                 Graph const &                other) const {
  // corresponding vertices (as mapped via indices_) have equal block counts,
  // contain equivalent blocks via colormap
  for (auto range_iter = permutable_block_ranges_.begin(),
            range_end  = permutable_block_ranges_.end();
       range_iter != range_end;
       ++range_iter) {
    for (auto [from, to] = *range_iter; from != to; ++from) {
      auto vertex1 = vertices_[indices_[from]];
      auto vertex2 = other.vertices_[from];
      if (not check_blocks(colormap, vertex1, vertex2)) {
        return false;
      }
    }
  }
  return true;
}

bool
Graph::equivalent_adjacency_matricies(Graph const & other) const {
  auto const & am1 = adjacency_matrix_;
  auto const & am2 = other.adjacency_matrix_;
  auto const   sz  = am1.size();
  assert(sz == am2.size());
  assert(indices_.size() == am1.size());

  for (Index i = 0; i < sz; ++i) {
    for (Index j = 0; j < sz; ++j) {
      if (am1.has_edge(indices_[i], indices_[j]) != am2.has_edge(i, j)) {
        return false;
      }
    }
  }
  return true;
}

void
Graph::dump() const {
  std::cout << "**** Graph ****\n"
            << matrix::WithNames{adjacency_matrix_, vertices_.names()} << '\n'
            << "perm range " << permutable_block_ranges_.size() << ".."
            << indices_.size() << ": [\n";
  for (auto [from, to] : permutable_block_ranges_) {
    std::cout << "  [" << from << ", " << to << "]\n";
  }
  std::cout << "]\nindices:";
  for (auto i : indices_) {
    std::cout << i << " ";
  }
  std::cout << std::endl;
}

bool
Graph::check_isomorphism(Graph const & other) const {
  std::iota(begin(indices_), end(indices_), 0);
  Graph::BlockEquivalenceMap colormap{};

  if (not check_basic_colorgroup_compatibility(*this, other, colormap)) {
    return false;
  }

  if (permutable_block_ranges_.empty()) {
    return equivalent_adjacency_matricies(other);
  }

  // Permute every ordering of each permutable block range, and check for
  // equivalence
  while (true) {
    for (auto range_idx_iter = permutable_block_ranges_.begin();
         not std::next_permutation(begin(indices_) + range_idx_iter->first,
                                   begin(indices_) + range_idx_iter->second);) {
      if (++range_idx_iter == permutable_block_ranges_.end()) {
        return false;
      }
      Graph::BlockEquivalenceMap dynamic_colormap;
      std::copy(std::begin(colormap), std::end(colormap), dynamic_colormap);

      if (check_dynamic_equivalence(dynamic_colormap, other)) {
        if (equivalent_adjacency_matricies(other)) {
          return true;
        }
      }
    }
  }
  return false;
}
