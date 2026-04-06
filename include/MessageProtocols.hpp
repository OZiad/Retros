#pragma once
#include <cstdint>

#pragma pack(push, 1)

struct PcapGlobalHeader {
  uint32_t magic_number;
  uint16_t version_major;
  uint16_t version_minor;
  int32_t thiszone;
  uint32_t sigfigs;
  uint32_t snaplen;
  uint32_t network;
};

struct PcapPacketHeader {
  uint32_t ts_sec;
  uint32_t ts_usec;
  uint32_t includedLen;
  uint32_t originalLen;
};

struct IEXTPHeader {
  uint8_t version;
  uint8_t reserved;
  uint16_t messageProtocolId;
  uint32_t channelId;
  uint32_t sessionId;
  uint16_t payloadLength;
  uint16_t messageCount;
  uint64_t streamOffset;
  uint64_t firstMsgSeqNum;
  uint64_t sendTime;
};

struct PriceLevelUpdate {
  uint8_t type;
  uint8_t eventFlags;
  uint64_t timestamp;
  char symbol[8];
  uint32_t size;
  int64_t price; // factor of 10,000
};

struct MarketMessage {
  char type;
  uint64_t orderId;
  uint32_t price;
  uint32_t qty;
};

#pragma pack(pop)
