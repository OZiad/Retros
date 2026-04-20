#include "LevelOrderBook.hpp"
#include <cstdint>
#include <optional>

LevelOrderBook::LevelOrderBook(const uint64_t initialMinPrice)
    : minPrice_{initialMinPrice} {
  poolIdxById_.reserve(10'000'000);
}

constexpr size_t LevelOrderBook::calculateIndex(const uint64_t price) const {
  return (price - minPrice_) / TICK_SIZE;
}

bool LevelOrderBook::insertPriceLevel(const PriceLevel &priceLevel) {
  return priceLevel.getSide() == Bid ? insertBidPriceLevel(priceLevel)
                                     : insertAskPriceLevel(priceLevel);
}

bool LevelOrderBook::updatePriceLevel(const uint64_t price, uint32_t size,
                                      Side side) {
  if (price < minPrice_) [[unlikely]] {
    return false;
  }

  const size_t index = calculateIndex(price);

  if (index >= priceLevelCount_) [[unlikely]] {
    return false;
  }

  auto &tracker = (side == Bid) ? bidPriceLevelTracker_ : askPriceLevelTracker_;
  auto &levels = (side == Bid) ? bidPriceLevels_ : askPriceLevels_;

  levels[index].setSize(size);
  levels[index].setPrice(price);

  tracker.setTick(index, size > 0);

  return true;
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

bool LevelOrderBook::insertBidPriceLevel(const PriceLevel &priceLevel) {
  uint64_t price = priceLevel.getPrice();
  if (priceLevel.getSide() != Side::Bid || price < minPrice_) [[unlikely]] {
    return false;
  }

  size_t index = calculateIndex(price);

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

  size_t index = calculateIndex(price);

  if (index < 0 || index >= priceLevelCount_) {
    return false;
  }

  askPriceLevels_[index] = priceLevel;
  askPriceLevelTracker_.setTick(index, true);

  return true;
}
