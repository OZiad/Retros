#include "DatabaseLogger.hpp"
#include "IEXParser.hpp"
#include "LevelOrderBook.hpp"
#include "MappedFile.hpp"
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <format>
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
  DatabaseLogger logger("dbname=retros user=omar "
                        "password=testPasswordForProject123 host=localhost");

  auto onPriceUpdate = [&](const std::string_view symbol, const uint64_t price,
                           const uint32_t size, const bool isBuySide,
                           const uint64_t timestamp) {
    auto [it, inserted] = symbolIdMap.try_emplace(
        symbol, static_cast<uint16_t>(symbolIdMap.size()));
    uint16_t id = it->second;

    if (inserted) {
      // naive but we anchor the book around the first price we see (minus $5)
      uint64_t anchorPrice = (price > 50000) ? (price - 50000) : 0;
      symbolOrderBooks.emplace_back(id, anchorPrice);
      std::cout << std::format(
          "Created order book for symbol: {}, anchor price: {}\n", symbol,
          anchorPrice);
    }

    if (isBuySide) {
      symbolOrderBooks[id].updatePriceLevel(price, size, Bid);
      std::cout << std::format(
          "inserting BUY pricelevel, price: {}, size: {}\n", price, size);
    } else {
      symbolOrderBooks[id].updatePriceLevel(price, size, Ask);
      std::cout << std::format(
          "inserting SELL pricelevel, price: {}, size: {}\n", price, size);
    }

    const auto bestBid = symbolOrderBooks[id].getMaxBidPriceLevel();
    const auto bestAsk = symbolOrderBooks[id].getMinAskPriceLevel();

    logger.push({static_cast<int64_t>(timestamp), std::string(symbol),
                 bestBid.getPrice(), bestBid.getSize(), bestAsk.getPrice(),
                 bestAsk.getSize()});
  };

  iex::parser::parseIEX(mappedFile.view(), onPriceUpdate);

  return 0;
}
