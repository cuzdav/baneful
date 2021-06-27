#pragma once

#include "Color.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace p1 {

  enum class Vertex : std::uint32_t{};

  class Verticies {
  public:
    using Names = std::vector<std::string>;
    using iterator = Names::iterator;
    using const_iterator = Names::const_iterator;
    using size_type = Names::size_type;

    Verticies() = default;

    size_type size() const;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    int index_of(std::string_view vertex, color::Color final_color) const;
    int index_of(std::string const& vertex, color::Color final_color) const;
    int index_of_checked(std::string_view vertex, color::Color final_color) const;
    int index_of_checked(std::string const& vertex, color::Color final_color) const;

    std::string const& name_of(int index) const;

    // returns index
    // transformed block has been adjusted by the transforms for fixups.
    int add_vertex_single(std::string_view vertex,
                          char transformed_block,
                          color::Color final_color);
    // returns index
    // transformed block has been adjusted by the transforms for fixups.
    int add_vertex_single(std::string const& vertex_name,
                          char transformed_block,
                          color::Color final_color);

    // create a synthetic vertex that is guaranteed unique.
    // Names are assigned, starting at _1 and counts up.
    // Returns index to it.
    int generate_unique_vertex_name();

    static std::string internal_name(std::string_view vertex_id_string, color::Color final_color);
  private:
    int index_of_internal(std::string_view internal_vertex_name) const;
    int index_of_checked_internal(std::string_view internal_vertex_name) const;

  private:
    Names vertex_names_;
    int next_unique_ = 0;
    Vertex verticies_[32]{};
  };

} // p1
