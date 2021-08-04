#pragma once

#include <utility>
#include <vector>

namespace algo {

void inline swap_idx_and_val(std::vector<std::size_t> & vec) {
  // Prereq: vec must be populated with N unique elements, in range 0-(N-1).

  // An array is like a hash from index->value, and this function reverses the
  // mapping: value->index, in linear time and O(1) extra storage, and no
  // allocation.

  // Add size to each element to indicate it is not yet processed
  const auto sz = size(vec);
  for (auto & e : vec) {
    e += sz;
  }

  for (int i = 0; i < sz; ++i) {
    auto idx = vec[i];

    // keep following chain as long as current index is not processed (value is
    // greater than size)
    for (int val = i; idx >= sz;) {
      int n = idx - sz;
      idx   = std::exchange(vec[n], val);
      val   = n;
    }
  }
}

} // namespace algo
