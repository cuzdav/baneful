#include "Color.hpp"
#include "enumutils.hpp"

#include "gtest/gtest.h"

namespace p1::color::test {

TEST(TestColor, toUnderlying) {
  EXPECT_EQ(0, +Color::TO);
  EXPECT_EQ(1, +Color::FROM);
  EXPECT_EQ(1<<1, +Color::SOLID_RECTANGLE);
  EXPECT_EQ(2<<1, +Color::WILDCARD);
  EXPECT_EQ(3<<1, +Color::BACKREF);
  EXPECT_EQ(4<<1, +Color::NEXT_CUSTOM);
}

  TEST(TestColor, logicalOR) {
    EXPECT_EQ(2, Color::TO | Color::DEFAULT);
    EXPECT_EQ(3, Color::FROM | Color::DEFAULT);

    EXPECT_EQ(, Color::TO | Color::DEFAULT);
    EXPECT_EQ(3, Color::FROM | Color::DEFAULT);
  }

} // namespace p1::color::test
