#pragma once

#include "Color.hpp"
#include "Vertex.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// A collection of named vertices, such that if paths logically converge they
// will use the same vertices, not duplicates.  Example:
//
// a -> bcd
// b -> cd
//
// The "cd" parts of both rules are identical "tails" and so can merge.

class Vertices {
public:
  using NameVec        = std::vector<std::string>;
  using VertexVec      = std::vector<vertex::Vertex>;
  using iterator       = NameVec::iterator;
  using const_iterator = NameVec::const_iterator;
  using size_type      = NameVec::size_type;

  Vertices();

  size_type      names_size() const;
  iterator       names_begin();
  iterator       names_end();
  const_iterator names_begin() const;
  const_iterator names_end() const;

  vertex::Vertex
  operator[](int idx) const {
    assert(idx < vertices_.size());
    return vertices_[idx];
  }

  int name_index_of(std::string_view  vertex,
                    color::FinalColor final_color) const;
  int name_index_of_checked(std::string_view  vertex,
                            color::FinalColor final_color) const;

  std::string const & name_of(int index) const;

  void
  set_vertex(int idx, vertex::Vertex v) {
    vertices_[idx] = v;
  }

  void sort();

  // Remove a vertex. This will change the vertex id numbers, moving the
  // highest-id into the place of the removed vertex. It will return the index
  // that was moved to fill in the hole.
  int remove_vertex(int idx);

  // vertex is the block-id of the vertex to create, and the first and
  // only block in it will be vertex[0]
  // returns index
  // transformed block has been adjusted by the transforms for fixups.
  int add_vertex_single(std::string_view vertex, block::FinalBlock block,
                        color::FinalColor final_color);

  static std::string internal_name(std::string_view  vertex_id_string,
                                   color::FinalColor final_color);

  int index_of_internal_name(std::string_view intern_vertex_name) const;
  int index_of_checked_internal_name(std::string_view intern_vertex_name) const;

private:
  constexpr static int DefaultCapacity = 16;

private:
  NameVec   vertex_names_;
  VertexVec vertices_{};
};
