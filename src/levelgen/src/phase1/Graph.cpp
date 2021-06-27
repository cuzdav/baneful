#include "Graph.hpp"
#include "Color.hpp"
#include "Transforms.hpp"
#include "Verticies.hpp"
#include "boost/json.hpp"

#include <cassert>
#include <filesystem>
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

  static void install_override_transforms(
      json::object const & type_overrides,
      Transforms& transforms)
  {
    for (auto const& [ch, type_override] : type_overrides) {
      if (size(ch) > 1) {
        throw std::runtime_error("Invalid type override, expecting char key, got: " + std::string(ch));
      }
      json::object const & type_config = type_override.as_object();
      assert(ch.size() == 1);
      transforms.add_level_type_override(ch[0], type_config);
    }
  }

  Graph::
  Graph(boost::json::object const & level_obj)
  {
    Transforms transforms;
    if (auto * type_overrides_val = level_obj.if_contains("type_overrides")) {
      install_override_transforms(type_overrides_val->as_object(), transforms);
    }

    Verticies verticies;
    if (auto * rules_val = level_obj.if_contains("rules")) {
      add_rules(rules_val->as_object(), transforms, verticies);
    }
    else {
      throw std::runtime_error("Level does not contain rules");
    }
  }

  void Graph::
  add_rules(
      json::object const & rules,
      Transforms & transforms,
      Verticies & verticies) {
    for (auto const& [from, to] : rules) {
      auto len = from.size();
      for (char const & ch : from) {
        transforms.add_vertex(std::string_view(&ch, len--), RuleSide::FROM, verticies);
      }
    }
  }


} // p1

