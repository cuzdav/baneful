#pragma once

/* A tiny DSL for generating json config for baneful levels in a more typesafe
 * way than raw strings parsed into json.
 *
 * example syntax:
 *
 auto lvl = level(
     rules(
         from("abc") = to("a", "b")
     ),
     type_overrides(
         std::pair{"a", rotating_colors("bcd")}
     )
 );

 This results in the following json:

 {
     "rules" : {
         "abc" : [
             "a",
             "b"
         ]
     },
     "type_overrides" : {
         "a" : {
             "type" : "RotatingColors",
             "cycle_chars" : "bcd"
         }
     }
 }

*/

namespace p1::test {

  template <typename... NVPair>
  auto level(NVPair... objects) {
    return boost::json::object{objects...};
  }

  auto rule(std::string_view from, boost::json::array to) {
    return std::pair(from, to);
  }

  auto rules(auto... rs) {
    return std::pair("rules", boost::json::object{rs...});
  }

  boost::json::array to(auto... args) {
    return boost::json::array{args...};
  }

  struct from {
    std::string_view from_sv_;
    from(std::string_view from_sv) : from_sv_{from_sv} { }
    auto operator=(boost::json::array to) {
      return rule(from_sv_, to);
    }
  };

  // customization for meaning of blocks.  Takes pair<SV, object>, where
  // SV is string-view to a letter representing a block, and <object> is
  // the configuration for what that type override can be.  Currently
  // only rotating_colors are supported.
  template <typename... StringViewT>
  auto type_overrides(std::pair<StringViewT, boost::json::object>... type_overrides) {
    return std::pair<boost::json::string_view, boost::json::object>{
      "type_overrides", boost::json::object { type_overrides... }
    };
  }


  // a type-override value generator
  auto rotating_colors(boost::json::string_view colors) {
    return boost::json::object{
      std::pair("type", "RotatingColors"),
      std::pair("cycle_chars", colors)
    };
  }

} // p1::test
