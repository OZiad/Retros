#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

constexpr int64_t IEX_SCALE = 10000;
constexpr int64_t TICK_SIZE = 0.01 * IEX_SCALE;

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
  uint32_t size;
};

class LevelOrderBook {

private:
  std::unordered_map<uint64_t, size_t> poolIdxById;
  size_t minAskIdx, maxBuyIdx;
  Order pool[10'000'000];
  // bitMaskTracker(poolSize/64);
  std::vector<size_t> free_indices;
};
