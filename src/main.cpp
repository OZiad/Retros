#include "IEXParser.hpp"
#include "LevelOrderBook.hpp"
#include "MappedFile.hpp"
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <sys/mman.h>
#include <unistd.h>

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  fs::path filePath = "resources/clean_data.pcap";
  MappedFile mappedFile(filePath);
  ankerl::unordered_dense::map<std::string_view, uint16_t> symbolIdMap;
  std::vector<LevelOrderBook> symbolOrderBooks;
  symbolOrderBooks.reserve(1000);

  auto onPriceUpdate = [&](const std::string_view symbol, const uint64_t price,
                           const uint32_t size, const bool isBuySide) {
    auto [it, inserted] = symbolIdMap.try_emplace(
        symbol, static_cast<uint16_t>(symbolIdMap.size()));
    uint16_t id = it->second;

    if (inserted) {
      // naive but we anchor the book around the first price we see (minus $5)
      uint64_t anchorPrice = (price > 50000) ? (price - 50000) : 0;
      symbolOrderBooks.emplace_back(id, anchorPrice);
    }

    if (isBuySide) {
      symbolOrderBooks[id].updatePriceLevel(price, size, Bid);
    } else {
      symbolOrderBooks[id].updatePriceLevel(price, size, Ask);
    }
    auto bestBid = symbolOrderBooks[id].getMaxBidPriceLevel();
    auto bestAsk = symbolOrderBooks[id].getMinAskPriceLevel();
    std::cout << std::format("[SYMBOL]: {}...[BEST BID]: PRICE: {}, SIZE: "
                             "{}...[BEST ASK]: PRICE: {}, SIZE: {}\n",
                             symbol, bestBid.getPrice(), bestBid.getSize(),
                             bestAsk.getPrice(), bestAsk.getSize());
  };

  iex::parser::parseIEX(mappedFile.view(), onPriceUpdate);

  return 0;
}
