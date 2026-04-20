#include "../include/TickBitMap.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>

class TickBitMapTest : public ::testing::Test {
protected:
  TickBitMap<512, 4> bitmap;
};

TEST_F(TickBitMapTest, SetTick_WithSingleBit_ThenIndicesMatch) {
  uint32_t tick = 500;

  bitmap.setTick(tick, true);
  EXPECT_EQ(bitmap.getMaxTickIndex(), tick);
  EXPECT_EQ(bitmap.getLowestTickIndex(), tick);

  bitmap.setTick(tick, false);
  EXPECT_FALSE(bitmap.getMaxTickIndex().has_value());
}

TEST_F(TickBitMapTest, SetTick_WhenClearingUnsetTick_ThenReturnsNullopt) {
  bitmap.setTick(0, false);
  EXPECT_FALSE(bitmap.getMaxTickIndex().has_value());
  EXPECT_FALSE(bitmap.getMaxTickIndex().has_value());
}

TEST_F(TickBitMapTest,
       SetTick_WithMultipleBitsInSameRow_ThenSummaryBitPersists) {
  bitmap.setTick(300, true);
  bitmap.setTick(400, true);

  bitmap.setTick(300, false);
  EXPECT_TRUE(bitmap.getMaxTickIndex().has_value());
  EXPECT_EQ(bitmap.getMaxTickIndex(), 400);

  bitmap.setTick(400, false);
  EXPECT_FALSE(bitmap.getMaxTickIndex().has_value());
}

TEST_F(TickBitMapTest,
       FindMaxTickIndex_WithMultipleSetBits_ThenReturnsHighestIndex) {
  bitmap.setTick(100, true);
  bitmap.setTick(1000, true);
  bitmap.setTick(500, true);

  auto maxTick = bitmap.getMaxTickIndex();
  ASSERT_TRUE(maxTick.has_value());
  EXPECT_EQ(*maxTick, 1000);
}

TEST_F(TickBitMapTest, SetTick_WithBoundaryIndices_ThenReturnsCorrectEdges) {
  uint32_t maxPossibleTick = (512 * 4 * 64) - 1;

  bitmap.setTick(0, true);
  EXPECT_EQ(bitmap.getLowestTickIndex(), 0);

  bitmap.setTick(maxPossibleTick, true);
  EXPECT_EQ(bitmap.getMaxTickIndex(), maxPossibleTick);
}

TEST_F(TickBitMapTest,
       SetTick_WithMultipleWordsInSameRow_ThenMinMaxAreCorrect) {
  bitmap.setTick(0, true);
  bitmap.setTick(64, true);

  EXPECT_EQ(bitmap.getLowestTickIndex(), 0);
  EXPECT_EQ(bitmap.getMaxTickIndex(), 64);
}
