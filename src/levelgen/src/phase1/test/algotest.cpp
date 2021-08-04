#include "sort.hpp"

#include "gtest/gtest.h"
#include <vector>
#include <cstddef>

void
naive_but_correct(std::vector<std::size_t> & v) {
  std::vector<std::size_t> v2(v.size());
  for (std::size_t i = 0, e = v.size(); i < e; ++i) {
    v2[v[i]] = i;
  }
  v = std::move(v2);
}

TEST(AlgoTest, test_swap_idx_and_val) {
  std::vector<std::size_t> v{0, 1, 2, 3};
  do {
    auto actual = v;
    algo::swap_idx_and_val(actual);

    auto expected = v;
    naive_but_correct(expected);
    EXPECT_EQ(expected, actual);
  } while (std::next_permutation(begin(v), end(v)));
}
