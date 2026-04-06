#include "MappedFile.hpp"
#include "MessageProtocols.hpp"
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

namespace fs = std::filesystem;

void parseIEX(std::string_view buffer) {
  const char *data = buffer.data();
  size_t offset = sizeof(PcapGlobalHeader);

  while (offset + sizeof(PcapPacketHeader) < buffer.size()) {
    auto pcapPacketHeader =
        reinterpret_cast<const PcapPacketHeader *>(data + offset);
    uint32_t packetDataLen = pcapPacketHeader->includedLen;
    const uint8_t networkHeaders = 42;
    size_t packetStart = offset + sizeof(PcapPacketHeader) + networkHeaders;

    if (packetStart + sizeof(IEXTPHeader) < packetStart + packetDataLen) {
      auto iexPakcetHdr =
          reinterpret_cast<const IEXTPHeader *>(data + packetStart);
      size_t msgPos = packetStart + sizeof(IEXTPHeader);

      for (uint16_t i = 0; i < iexPakcetHdr->messageCount; ++i) {
        {
          auto msgSize = reinterpret_cast<const uint16_t *>(data + msgPos);
          msgPos += 2;
          uint8_t msgType = *(data + msgPos);

          if (msgType == '5' || msgType == '8') {
            auto priceLevelUpdate =
                reinterpret_cast<const PriceLevelUpdate *>(data + msgPos);
            std::string_view symbol(priceLevelUpdate->symbol, 8);
            double realPrice = priceLevelUpdate->price / 10000.0;

            std::cout << std::format("[BOOK priceLevelUpdate] msgType: {}, "
                                     "Symbol: {} Size: {}, Price: ${}\n",
                                     char(msgType), symbol,
                                     priceLevelUpdate->size, realPrice);
          }
          msgPos += *msgSize;
        }
      }
    }
    offset = packetStart + packetDataLen;
  }
}

int main(int argc, char **argv) {
  fs::path filePath = "resources/clean_data.pcap";
  MappedFile mappedFile(filePath);
  parseIEX(mappedFile.view());
}
