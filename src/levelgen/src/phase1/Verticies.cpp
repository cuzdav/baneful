#include "Verticies.hpp"

#include "RuleSide.hpp"
#include <cassert>
#include <stdexcept>

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

The vertex class originally creates graphs in ONE-BLOCK-PER-CHAR in the grap, meaning there is no initial merging.  That comes at a later optimizing pass, because while we are reading in the graph it's too early to merge.  We do not (yet) knoow which parts are shared and which are eligible for merging.

*/

  // "final_color" is normal color with the the From/To bit already set (or
  // unset) properly
  static Vertex
  make_vertex_single(color::Color final_color, char transformed_block) {

    // an optimized/merged vertex may have multiple blocks within
    // like this:
    // [???][XYYYY][AAAA][BBBB][CCCC][DDDD][EEEE][FFFF]
    //      assert(+actual_color < 32); // Only allotted 5 bits
    // set XYYYY
    std::uint32_t vertex = static_cast<std::uint32_t>(final_color) << 24;

    // only set [AAAA] (subsequent blocks can be added later)
    assert(transformed_block < 16);
    vertex |= std::uint32_t(transformed_block) << 20;
    return Vertex{vertex};
  }

    Verticies::size_type Verticies::
  size() const {
    return std::size(vertex_names_);
  }

  Verticies::iterator Verticies::
  begin() {
    return std::begin(vertex_names_);
  }

  Verticies::iterator Verticies::
  end() {
    return std::end(vertex_names_);
  }

  Verticies::const_iterator Verticies::
  begin() const {
    return std::begin(vertex_names_);
  }

  Verticies::const_iterator Verticies::
  end() const {
    return std::end(vertex_names_);
  }

  int Verticies::
  index_of(std::string_view vertex_name) const {
    auto it = std::find(begin(), end(), vertex_name);
    if (it == end()) {
      return -1;
    }
    return it - begin();
  }

  int Verticies::
  index_of(std::string const& vertex_name) const {
    return index_of(std::string_view(vertex_name));
  }

  int Verticies::
  index_of_checked(std::string_view vertex_name) const {
    int idx = index_of(vertex_name);
    if (idx == -1) {
      throw std::runtime_error(
          "vertex " +
          std::string{vertex_name.data(), vertex_name.size()} +
          " unknown");
    }
    return idx;
  }

  int Verticies::
  index_of_checked(std::string const& vertex_name) const {
    return index_of_checked(vertex_name);
  }

  // create a synthetic vertex that is guaranteed unique.
  // Names are assigned, starting at _1 and counts up.
  int Verticies::
  generate_unique_vertex_name() {
    std::string name = "_" + std::to_string(++next_unique_);
    vertex_names_.push_back(name);
    return vertex_names_.size() - 1;
  }

  int Verticies::
  add_vertex_single(std::string_view vertex_name,
             char transformed_block,
             color::Color final_color) {
    int idx = index_of(vertex_name);

    if (idx == -1) {
      idx = size();
      vertex_names_.emplace_back(vertex_name.data(), vertex_name.size());
      verticies_[idx] = make_vertex_single(final_color,
                                           transformed_block);
    }
    return idx;
  }

  int Verticies::
  add_vertex_single(std::string const& vertex_name,
                    char transformed_block,
                    color::Color color) {
    return add_vertex_single(std::string_view{vertex_name},
                             transformed_block,
                             color);
  }
} // p1

