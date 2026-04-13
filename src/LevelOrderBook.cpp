#include "LevelOrderBook.hpp"
#include <cstddef>
#include <cstdint>

LevelOrderBook::LevelOrderBook() { poolIdxById_.reserve(10'000'000); }

bool LevelOrderBook::addPriceLevel(const PriceLevel &priceLevel) {
  if (priceLevel.getSide() == Buy) {
    return insertBuyPriceLevel(priceLevel);
  } else {
    return insertSellPriceLevel(priceLevel);
  }
}

bool LevelOrderBook::insertSellPriceLevel(const PriceLevel &priceLevel) {
  if (priceLevel.getSide() != Side::Sell) {
    return false;
  }

  uint64_t price = priceLevel.getPrice();

  // TODO: check if this index is already a tick
  size_t index =
      (price - this->minPrice_) / TICK_SIZE; // 100 if price is in cents*100

  if (index < 0 || index >= priceLevelCount_) {
    return false;
  }

  sellPriceLevels_[index] = priceLevel;

  sellPriceLevelTracker_.setTick(index, true);

  return true;
  return false;
}
