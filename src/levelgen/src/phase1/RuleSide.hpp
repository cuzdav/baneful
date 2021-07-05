#pragma once

#include <string>

namespace p1 {

  // a -> b
  //
  // a is FROM, b is TO
  // all blocks are marked as FROM or TO

  enum class RuleSide : bool {
    TO   = 0,
    FROM = 1
  };

  inline std::string to_string(RuleSide side) {
    return side == RuleSide::TO ? "TO" : side == RuleSide::FROM ? "FROM" : "???";
  }

}
