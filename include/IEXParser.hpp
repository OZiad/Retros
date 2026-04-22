#pragma once
#include "MessageProtocols.hpp"
#include <string_view>

namespace iex::parser {

template <typename Callback>
inline void parseIEX(std::string_view buffer, Callback &&onUpdate) {
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
            onUpdate(symbol, priceLevelUpdate->price, priceLevelUpdate->size,
                     priceLevelUpdate->type);
          }
          msgPos += *msgSize;
        }
      }
    }
    offset = packetStart + packetDataLen;
  }
}
} // namespace iex::parser
