#pragma once

#include <cstdint>
#include <string>

struct MarketSnapshot {
  int64_t timestamp;
  std::string symbol;
  uint64_t bidPrice;
  uint32_t bidSize;
  uint64_t askPrice;
  uint32_t askSize;
};
