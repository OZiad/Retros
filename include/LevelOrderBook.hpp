#pragma once

#include "TickBitMap.hpp"
#include "ankler/unordered_dense.h"
#include <cstdint>

constexpr int64_t IEX_SCALE = 10'000;
constexpr int64_t TICK_SIZE = IEX_SCALE / 100;

enum Side : char { Bid, Ask };

struct Order {
  double price;
  uint64_t id;
  uint32_t quantity;
  int prev, next;
  bool isBid;
  char padding[3];
};

struct PriceLevel {
private:
  uint64_t price;
  size_t headIdx;
  uint32_t size;
  Side side;

public:
  PriceLevel(const PriceLevel &) = default;
  PriceLevel() = default;
  PriceLevel(PriceLevel &&) = default;
  PriceLevel &operator=(const PriceLevel &) = default;
  PriceLevel &operator=(PriceLevel &&) = default;

  constexpr PriceLevel(uint64_t price, size_t headIdx, uint32_t size, Side side)
      : price(price), headIdx(headIdx), size(size), side(side) {}

  uint64_t getPrice() const { return price; }
  size_t getHeadIndex() const { return headIdx; }
  uint32_t getSize() const { return size; }
  Side getSide() const { return side; }
  void setHeadIndex(size_t newHeadIdx) { headIdx = newHeadIdx; }
  void setSize(uint32_t newSize) { size = newSize; }
};

class LevelOrderBook {

private:
  uint64_t minPrice_{};
  ankerl::unordered_dense::map<uint64_t, size_t> poolIdxById_{};
  static constexpr long long int priceLevelCount_ = 4096;
  std::array<PriceLevel, priceLevelCount_> bidPriceLevels_;
  std::array<PriceLevel, priceLevelCount_> askPriceLevels_;
  TickBitMap<> bidPriceLevelTracker_;
  TickBitMap<> askPriceLevelTracker_;
  inline static constexpr PriceLevel NULL_LEVEL{0, 0, 0, Side::Bid};

  bool insertBidPriceLevel(const PriceLevel &priceLevel_);
  bool insertAskPriceLevel(const PriceLevel &priceLevel_);

public:
  LevelOrderBook();
  bool insertPriceLevel(const PriceLevel &priceLevel);
  const PriceLevel &getMaxBidPriceLevel() const;
  const PriceLevel &getMinAskPriceLevel() const;
  void updatePriceLevel(const uint64_t price, uint32_t size, Side side);
};
