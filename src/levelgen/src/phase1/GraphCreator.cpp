#include "GraphCreator.hpp"
#include "Color.hpp"
#include "Transforms.hpp"
#include "Vertices.hpp"
#include <boost/json.hpp>

#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

using namespace boost;
using namespace std::literals;

/*
  If "a", "b", and "c" were
  standard Rectangle types, then "abc" would be a single
  Rectangle(abc). But if "." is the wildcard, that's a different
  color from 'a' and 'c', so they must be represented in multiple
  vertices.
  "a.c" would be 3 nodes:Rectangle(a)--Wildcard--Rectangle(c)

  if a is Cycle(bcd)
  then bcda ::= Rectangle(bcd)--Cycle(bcd)

  A color modifier also notates whether a vertex is part of the
  "from" pattern or the "to" value (original idea of lvalue or
  rvalue?) so we add an extra logical argument to the notation:

  Thus in json, a rule like

  "rules": {
      "a": ["a"]
  }


  we differentiate the "from" pattern and a "to" pattern by
  annotating the color

  Implementation wise, Rectangle, Cycle, and Wildcard (etc) are
  colors represented as simple numbers, and FROM and TO annotations
  are the high bit in the color. "From" bits are 1, "To" bits
  are 0.

  If we can have 8 block names (a-g, plus "empty") then we need 3 bits to
  represent each block value.  We can fill unused blocks
  with the bit pattern 000. We then reserve 5 bits for color--4 bits
  for category, and 1 bit for from/to annotation. That uses (8*3) +
  5 = 29 bits, leaving 3 more for flexibility.

*/

namespace p1 {

static void
install_override_transforms(json::object const & type_overrides,
                            Transforms &         transforms) {
  for (auto const & [ch, type_override] : type_overrides) {
    if (size(ch) > 1) {
      throw std::runtime_error(
          "Invalid type override, expecting char key, got: " + std::string(ch));
    }
    json::object const & type_config = type_override.as_object();
    assert(ch.size() == 1);
    transforms.add_level_type_override(ch[0], type_config);
  }
}

GraphCreator::GraphCreator(boost::json::object const & level_obj) {
  if (auto * type_overrides_val = level_obj.if_contains("type_overrides")) {
    install_override_transforms(type_overrides_val->as_object(), transforms_);
  }

  if (auto * rules_val = level_obj.if_contains("rules")) {
    add_rules(rules_val->as_object());
  }
  else {
    throw std::runtime_error("Level does not contain rules");
  }
}

int
GraphCreator::process_chain(json::string_view chain, RuleSide side,
                            int prev_idx, GraphCreator::TraverseAction action) {
  if (chain.empty()) {
    assert(side == RuleSide::TO); // no empty chains on the left
    chain = block::NOTHING_BLOCK_CSTR;
  }
  auto len = chain.size();
  int  idx;
  for (char const & ch : chain) {
    auto vertex_id      = std::string_view(&ch, len--);
    auto [block, color] = transforms_.do_transform(vertex_id, side);
    idx                 = vertices_.add_vertex_single(vertex_id, block, color);

    if (action == TraverseAction::CREATE_EDGES) {
      assert(adjacency_matrix_.has_value());
      if (prev_idx != -1) {
        adjacency_matrix_->add_edge(prev_idx, idx);
      }
      prev_idx = idx;
    }
  }
  return idx;
}

void
GraphCreator::traverse_input(json::object const &         rules,
                             GraphCreator::TraverseAction action) {
  for (auto const & [from, to] : rules) {
    int prev_idx = process_chain(from, RuleSide::FROM, -1, action);

    // Each chain's prev_idx is from above; do NOT update it in the loop.
    // "a"->["b", "c"], then "a" is the prev of both "b" and "c"
    for (json::value to : to.as_array()) {
      process_chain(to.as_string(), RuleSide::TO, prev_idx, action);
    }
  }
}

void
GraphCreator::add_rules(json::object const & rules) {
  traverse_input(rules, TraverseAction::CREATE_VERTEX_ONLY);
  // must know the number of vertices before we can size the adj matrix
  adjacency_matrix_.emplace(vertices_.names_size());
  traverse_input(rules, TraverseAction::CREATE_EDGES);
}

void
GraphCreator::compress_vertices() {
  int  am_size = adjacency_matrix_->size();
  bool something_changed;
  do {
    something_changed = false;
    for (int cur_idx = 0; cur_idx < am_size; ++cur_idx) {
      int source_idx;
      int indegree = adjacency_matrix_->visit_parents_of(
          cur_idx, [&](int src_idx) { source_idx = src_idx; });
      if (indegree == 1) {
        something_changed = try_to_merge(source_idx, cur_idx);
      }
    }
  } while (something_changed);
}

bool
GraphCreator::try_to_merge(int from_idx, int to_idx) {
  vertex::Vertex from_vtx = vertices_[from_idx];
  vertex::Vertex to_vtx   = vertices_[to_idx];

  bool something_changed = false;

  if (same_color(from_vtx, to_vtx)) {
    vertex::Vertex merged_vtx = create_merged(from_vtx, to_vtx);
    if (merged_vtx != from_vtx) {
      something_changed = true;
      int  num_merged   = num_can_merge(from_vtx, to_vtx);
      auto to_vtx2      = pop_front(to_vtx, num_merged);
      vertices_.set_vertex(from_idx, merged_vtx);
      vertices_.set_vertex(to_idx, to_vtx2);
      if (size(to_vtx2) == 0) {
        remove_vertex(to_idx, from_idx);
      }
    }
  }
  return something_changed;
}

void
GraphCreator::remove_vertex(int doomed_idx, int parent_idx) {
  // clang-format off
  // vertex became empty, unthread it from adjacency matrix.
  // 1) give its out-edges to its parent
  // 2) remove parent's edge to child
  // 3) remove vertex from Vertices name-map.  This will move the "last"
  //    vertex into the dead slot.  Ex: if there are 4 vertices (0..3) and
  //    we remove 1, then 3 takes the empty slot opened by removing 1
  // 4) Thus, we must update the adjacency matrix edges to follow where that
  //    node moved.
  // clang-format on

  assert(adjacency_matrix_->indegree_of(doomed_idx) == 1);

  give_vertex_out_edges_to_parent(doomed_idx, parent_idx);
  adjacency_matrix_->remove_edge(parent_idx, doomed_idx);
  int moved_idx = vertices_.remove_vertex(doomed_idx);
  if (moved_idx != -1 && moved_idx != doomed_idx) {
    vertex_moved(moved_idx, doomed_idx);
  }
}

void
GraphCreator::give_vertex_out_edges_to_parent(int vertex_idx, int parent_idx) {
  adjacency_matrix_->visit_children_of(
      vertex_idx, [parent_idx, this](int child_idx) {
        adjacency_matrix_->add_edge(parent_idx, child_idx);
      });
}

void
GraphCreator::vertex_moved(int old_idx, int new_idx) {
  adjacency_matrix_->visit_parents_of(
      old_idx, [new_idx, old_idx, this](int parent_idx) {
        adjacency_matrix_->add_edge(parent_idx, new_idx);
        adjacency_matrix_->remove_edge(parent_idx, old_idx);
      });
}

} // namespace p1
