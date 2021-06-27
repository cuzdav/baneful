#pragma once

namespace p1 {

  // a -> b
  //
  // a is FROM, b is TO
  // all blocks are marked as FROM or TO

enum class RuleSide : bool {
  TO   = 0,
  FROM = 1
};

}
