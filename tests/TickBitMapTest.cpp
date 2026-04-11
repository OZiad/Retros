#include "../include/TickBitMap.hpp"
#include <gtest/gtest.h>

class TickBitMapTest : public ::testing::Test {
protected:
  TickBitMap<512, 4> bitmap;
};

TEST_F(TickBitMapTest, SetAndClearTick) {
  uint32_t tick = 500;

  bitmap.setTick(tick, true);
  EXPECT_EQ(bitmap.findMaxTickIndex(), tick);
  EXPECT_EQ(bitmap.getLowestTickIndex(), tick);

  bitmap.setTick(tick, false);
  EXPECT_FALSE(bitmap.findMaxTickIndex().has_value());
}

// since row 1 spans 256-511, setting two ticks in the same row
// should keep the summary bit active until both are cleared.
TEST_F(TickBitMapTest, SummaryBitPersistence) {
  bitmap.setTick(300, true);
  bitmap.setTick(400, true);

  bitmap.setTick(300, false);
  // row 1 is still active because tick 400 is set
  EXPECT_TRUE(bitmap.findMaxTickIndex().has_value());
  EXPECT_EQ(bitmap.findMaxTickIndex(), 400);

  bitmap.setTick(400, false);
  EXPECT_FALSE(bitmap.findMaxTickIndex().has_value());
}

TEST_F(TickBitMapTest, FindMaxTick) {
  bitmap.setTick(100, true);
  bitmap.setTick(1000, true);
  bitmap.setTick(500, true);

  auto maxTick = bitmap.findMaxTickIndex();
  ASSERT_TRUE(maxTick.has_value());
  EXPECT_EQ(*maxTick, 1000);
}

TEST_F(TickBitMapTest, BoundaryTicks) {
  uint32_t maxPossibleTick = (512 * 4 * 64) - 1;

  bitmap.setTick(0, true);
  EXPECT_EQ(bitmap.getLowestTickIndex(), 0);

  bitmap.setTick(maxPossibleTick, true);
  EXPECT_EQ(bitmap.findMaxTickIndex(), maxPossibleTick);
}

TEST_F(TickBitMapTest, MultipleWordsSameRow) {
  bitmap.setTick(0, true);  // row 0, col 0, bit 0
  bitmap.setTick(64, true); // row 0, col 1, bit 0

  EXPECT_EQ(bitmap.getLowestTickIndex(), 0);
  EXPECT_EQ(bitmap.findMaxTickIndex(), 64);
}
