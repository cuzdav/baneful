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
  verticies.
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
    idx                 = verticies_.add_vertex_single(vertex_id, block, color);

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
  adjacency_matrix_.emplace(verticies_.names_size());
  traverse_input(rules, TraverseAction::CREATE_EDGES);
}

void
GraphCreator::merge_like_verticies() {
  for (int i = 0, sz = adjacency_matrix_->size(); i < sz; ++i) {
    int source_idx = -1;
    int indegree   = adjacency_matrix_->visit_sources_of(
          i, [&](int src_idx) { source_idx = src_idx; });
    vertex::Vertex from_vtx = verticies_[source_idx];
    vertex::Vertex to_vtx   = verticies_[i];
    if (indegree == 1 && can_merge(from_vtx, to_vtx)) {}
  }
}

} // namespace p1
