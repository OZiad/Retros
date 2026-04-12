#include "../include/TickBitMap.hpp"
#include <gtest/gtest.h>

class TickBitMapTest : public ::testing::Test {
protected:
  TickBitMap<512, 4> bitmap;
};

TEST_F(TickBitMapTest, SetTick_WithSingleBit_ThenIndicesMatch) {
  uint32_t tick = 500;

  bitmap.setTick(tick, true);
  EXPECT_EQ(bitmap.findMaxTickIndex(), tick);
  EXPECT_EQ(bitmap.getLowestTickIndex(), tick);

  bitmap.setTick(tick, false);
  EXPECT_FALSE(bitmap.findMaxTickIndex().has_value());
}

TEST_F(TickBitMapTest, SetTick_WhenClearingUnsetTick_ThenReturnsNullopt) {
  bitmap.setTick(0, false);
  EXPECT_FALSE(bitmap.findMaxTickIndex().has_value());
  EXPECT_FALSE(bitmap.findMaxTickIndex().has_value());
}

TEST_F(TickBitMapTest,
       SetTick_WithMultipleBitsInSameRow_ThenSummaryBitPersists) {
  bitmap.setTick(300, true);
  bitmap.setTick(400, true);

  bitmap.setTick(300, false);
  EXPECT_TRUE(bitmap.findMaxTickIndex().has_value());
  EXPECT_EQ(bitmap.findMaxTickIndex(), 400);

  bitmap.setTick(400, false);
  EXPECT_FALSE(bitmap.findMaxTickIndex().has_value());
}

TEST_F(TickBitMapTest,
       FindMaxTickIndex_WithMultipleSetBits_ThenReturnsHighestIndex) {
  bitmap.setTick(100, true);
  bitmap.setTick(1000, true);
  bitmap.setTick(500, true);

  auto maxTick = bitmap.findMaxTickIndex();
  ASSERT_TRUE(maxTick.has_value());
  EXPECT_EQ(*maxTick, 1000);
}

TEST_F(TickBitMapTest, SetTick_WithOutOfBoundsIndex_ThenOperationIsIgnored) {
  uint32_t tick = (512 * 4 * 64) * 2;
  bitmap.setTick(tick, true);
  auto maxTick = bitmap.findMaxTickIndex();
  ASSERT_FALSE(maxTick.has_value());
}

TEST_F(TickBitMapTest, SetTick_WithBoundaryIndices_ThenReturnsCorrectEdges) {
  uint32_t maxPossibleTick = (512 * 4 * 64) - 1;

  bitmap.setTick(0, true);
  EXPECT_EQ(bitmap.getLowestTickIndex(), 0);

  bitmap.setTick(maxPossibleTick, true);
  EXPECT_EQ(bitmap.findMaxTickIndex(), maxPossibleTick);
}

TEST_F(TickBitMapTest,
       SetTick_WithMultipleWordsInSameRow_ThenMinMaxAreCorrect) {
  bitmap.setTick(0, true);  // row 0, col 0, bit 0
  bitmap.setTick(64, true); // row 0, col 1, bit 0

  EXPECT_EQ(bitmap.getLowestTickIndex(), 0);
  EXPECT_EQ(bitmap.findMaxTickIndex(), 64);
}
