#include "LevelOrderBook.hpp"
#include <optional>

LevelOrderBook::LevelOrderBook() { poolIdxById_.reserve(10'000'000); }

bool LevelOrderBook::insertPriceLevel(const PriceLevel &priceLevel) {
  return priceLevel.getSide() == Bid ? insertBidPriceLevel(priceLevel)
                                     : insertAskPriceLevel(priceLevel);
}

const PriceLevel &LevelOrderBook::getMaxBidPriceLevel() const {
  std::optional<size_t> maxTickIndex = bidPriceLevelTracker_.getMaxTickIndex();
  if (maxTickIndex) {
    return bidPriceLevels_[*maxTickIndex];
  }

  return NULL_LEVEL;
}

const PriceLevel &LevelOrderBook::getMinAskPriceLevel() const {
  std::optional<size_t> minTickIndex =
      askPriceLevelTracker_.getLowestTickIndex();
  if (minTickIndex) {
    return bidPriceLevels_[*minTickIndex];
  }

  return NULL_LEVEL;
}

void updatePriceLevel(const uint64_t price, uint32_t size, Side side) {
  // TODO
}

bool LevelOrderBook::insertBidPriceLevel(const PriceLevel &priceLevel) {
  uint64_t price = priceLevel.getPrice();
  if (priceLevel.getSide() != Side::Bid || price < minPrice_) [[unlikely]] {
    return false;
  }

  size_t index = (price - minPrice_) / TICK_SIZE;

  if (index < 0 || index >= priceLevelCount_) {
    return false;
  }

  bidPriceLevels_[index] = priceLevel;
  bidPriceLevelTracker_.setTick(index, true);

  return true;
}

bool LevelOrderBook::insertAskPriceLevel(const PriceLevel &priceLevel) {
  uint64_t price = priceLevel.getPrice();
  if (priceLevel.getSide() != Side::Ask || price < minPrice_) [[unlikely]] {
    return false;
  }

  size_t index = (price - minPrice_) / TICK_SIZE;

  if (index < 0 || index >= priceLevelCount_) {
    return false;
  }

  askPriceLevels_[index] = priceLevel;
  askPriceLevelTracker_.setTick(index, true);

  return true;
}
