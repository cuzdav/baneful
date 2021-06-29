#pragma once

#include "Color.hpp"
#include "Vertex.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace p1 {


  class Verticies {
  public:
    using Names = std::vector<std::string>;
    using iterator = Names::iterator;
    using const_iterator = Names::const_iterator;
    using size_type = Names::size_type;

    Verticies() = default;

    size_type names_size() const;
    iterator names_begin();
    iterator names_end();
    const_iterator names_begin() const;
    const_iterator names_end() const;

    int name_index_of(std::string_view vertex, color::Color final_color) const;
    int name_index_of_checked(std::string_view vertex, color::Color final_color) const;

    std::string const& name_of(int index) const;

    // vertex is the block-id of the vertex to create, and the first and
    // only block in it will be vertex[0]
    // returns index
    // transformed block has been adjusted by the transforms for fixups.
    int add_vertex_single(std::string_view vertex,
                          block::FinalBlock block,
                          color::Color final_color);
    // create a synthetic vertex that is guaranteed unique.
    // Names are assigned, starting at _1 and counts up.
    // Returns index to it.
    int generate_unique_vertex_name();

    static std::string internal_name(std::string_view vertex_id_string, color::Color final_color);
  private:
    int name_index_of_internal(std::string_view internal_vertex_name) const;
    int name_index_of_checked_internal(std::string_view internal_vertex_name) const;

  private:
    Names vertex_names_;
    int next_unique_ = 0;
    vertex::Vertex verticies_[32]{};
  };

} // p1
