#include "Vertices.hpp"

#include "Block.hpp"
#include "RuleSide.hpp"
#include <cassert>
#include <stdexcept>
#include <string_view>

#include <iostream>

namespace p1 {

/*
memory layout
[???][XYYYY][AAAA][BBBB][CCCC][DDDD][EEEE][FFFF]
Each letter (or ?) above is a bit.  The [...] is just a notation
to grou them.

[???] reserved
[XYYYY] - color, X is 1 if From, 0 if To
[AAAA]...[FFFF] each group A..F is 4 bits each per block of same color.

Nodes like the above represent a single vertex. Adjacent blocks
can be combined into a single node if they fit, or chained
through multiple vertexes. While not expected to happen, if we
had a sequence of more than 6 blocks of the same color, they
wouldn't all fit in the above memory scheme, but we could have
additional 32-bit memory structures chained in the graph,
as needed.

The ENTIRE chain from a node to the end is considered its
identity. For building the graph, for example, we cannot take a
block in isolate, but must consider everything that follows it as
part of what it is.

Given a rule like this:
{
   "b": ["abc", "a"]
}

we have "b goes to abc or to a", but notice that both start with "a",
and the 'a' cannot be shared.  To draw this as a graph:

F:b -> T:a -> T:b -> T:c

the node "T:a" is not able to do double duty or we lose the fact
that F:b -> T:a by itself is valid.  Thus must represent it like this:

F:b -> T:a -> T:b -> T:c
    -> T:a

If we expand the rule to include some duplicate targets:
{
   "a": ["bc"],
   "b": ["abc", "a"]
}

We then can see that the "bc" is the same since it is the "tail"
of two chains and so have no difference.

F:a -> T:b -> T:c
F:b -> T:a -> T:b -> T:c
F:b -> T:a

Adding whitespace

F:a ->        T:b -> T:c
F:b -> T:a -> T:b -> T:c
F:b -> T:a

Thus, T:b -> T:c can be shared, used by both F:a and F:b

F:a ---------->T:b -> T:c
               ^
              /
             /
F:b -+--> T:a
     |
     \--> T:a

Further, since T:c only has one edge into it, it can merge into T:b since they
both have the same color (Rectangle)

F:a ---------->T:bc
             ^
            /
           /
F:b -+--> T:a
   |
   \--> T:a


It is the job of the transforms to separate the logical rules into
their actual vertex based on the color (type overrides) described.

The vertex class originally creates graphs in ONE-BLOCK-PER-CHAR in the grap,
meaning there is no initial merging.  That comes at a later optimizing pass,
because while we are reading in the graph it's too early to merge.  We do not
(yet) knoow which parts are shared and which are eligible for merging.

*/

Vertices::size_type
Vertices::names_size() const {
  return std::size(vertex_names_);
}

Vertices::iterator
Vertices::names_begin() {
  return std::begin(vertex_names_);
}

Vertices::iterator
Vertices::names_end() {
  return std::end(vertex_names_);
}

Vertices::const_iterator
Vertices::names_begin() const {
  return std::begin(vertex_names_);
}

Vertices::const_iterator
Vertices::names_end() const {
  return std::end(vertex_names_);
}

int
Vertices::name_index_of(std::string_view  vertex_name,
                        color::FinalColor final_color) const {
  auto internal_vertex_name = internal_name(vertex_name, final_color);
  return index_of_internal_name(internal_vertex_name);
}

int
Vertices::index_of_internal_name(std::string_view internal_vertex_name) const {
  auto it = std::find(names_begin(), names_end(), internal_vertex_name);
  if (it == names_end()) {
    return -1;
  }
  std::cout << "Found, it was at " << (it - names_begin()) << ": " << *it
            << "\n";
  return it - names_begin();
}

int
Vertices::name_index_of_checked(std::string_view  vertex_name,
                                color::FinalColor final_color) const {
  auto internal_vertex_name = internal_name(vertex_name, final_color);
  int  idx = index_of_checked_internal_name(internal_vertex_name);
  if (idx == -1) {
    throw std::runtime_error("vertex " + internal_vertex_name + " unknown");
  }
  return idx;
}

int
Vertices::index_of_checked_internal_name(
    std::string_view internal_vertex_name) const {
  int idx = index_of_internal_name(internal_vertex_name);
  if (idx == -1) {
    auto msg = "vertex " + std::string(internal_vertex_name) + " unknown";
    throw std::runtime_error(msg);
  }
  return idx;
}

std::string const &
Vertices::name_of(int index) const {
  return vertex_names_[index];
}

int
Vertices::add_vertex_single(std::string_view  vertex_name,
                            block::FinalBlock transformed_block,
                            color::FinalColor final_color) {
  auto internal_vertex_name = internal_name(vertex_name, final_color);
  int  idx                  = index_of_internal_name(internal_vertex_name);

  if (idx == -1) {
    idx = names_size();
    assert(idx < vertices_.size());
    vertex_names_.emplace_back(internal_vertex_name);
    vertices_[idx] = vertex::create(final_color, transformed_block);
  }
  return idx;
}

std::string
Vertices::internal_name(std::string_view  vertex_id_string,
                        color::FinalColor final_color) {
  // convert color to printable char and prefix the name with it.
  return color_to_char(final_color) + std::string(vertex_id_string);
}

int
Vertices::remove_vertex(int idx) {
  if (vertex_names_.empty()) {
    return -1;
  }

  int last_idx = vertex_names_.size() - 1;
  if (last_idx != idx) {
    vertex_names_[idx] = vertex_names_[last_idx];
    vertices_[idx]     = vertices_[last_idx];
  }
  vertex_names_.pop_back();

  return last_idx;
}

} // namespace p1
