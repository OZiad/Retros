#pragma once

#include "TickBitMap.hpp"
#include <cstdint>
#include <unordered_map>

constexpr int64_t IEX_SCALE = 10000;
constexpr int64_t TICK_SIZE = IEX_SCALE / 100;

struct Order {
  double price;
  uint64_t id;
  uint32_t quantity;
  int prev, next;
  bool isBuy;
  char padding[3];
};

struct PriceLevel {
private:
  uint64_t price;
  size_t headIdx;
  uint32_t size;
};

class LevelOrderBook {

private:
  std::unordered_map<uint64_t, size_t> poolIdxById;
  size_t minAskIdx, maxBuyIdx;
  Order pool[10'000'000]; // TODO defuse this bomb
  static constexpr long long int priceLevelCount = 4096;
  PriceLevel BuyPriceLevels[priceLevelCount];
  PriceLevel sellPriceLevels[priceLevelCount];
  TickBitMap<> buyPriceLevelTracker;
  TickBitMap<> sellPriceLevelTracker;
};
