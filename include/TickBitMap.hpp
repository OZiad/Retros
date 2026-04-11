#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>

template <size_t NumRows = 512, size_t WordsPerRow = 4> class TickBitMap {

private:
  static constexpr size_t BITS_PER_WORD = 64;
  static constexpr size_t BITS_PER_ROW = WordsPerRow * BITS_PER_WORD;
  std::array<std::array<uint64_t, WordsPerRow>, NumRows> words{};
  std::array<uint64_t, (NumRows + 64 - 1) / 64> // ceiling of numRows/64
      wordsSummary{};

public:
  void setTick(uint32_t tick, bool value) {
    size_t row = tick / BITS_PER_ROW;
    size_t col = (tick % BITS_PER_ROW) / BITS_PER_WORD;
    size_t pos = tick % BITS_PER_WORD;
    uint64_t bit = (1ULL << pos);
    uint64_t summaryBit = (1ULL << row % 64);

    if (value) {
      words[row][col] |= bit;
      wordsSummary[row / 64] |= summaryBit;
    } else {
      words[row][col] &= ~bit;
      bool isRowEmpty = true;
      for (auto const &w : words[row]) {
        if (w != 0) {
          isRowEmpty = false;
          break;
        }
      }

      if (isRowEmpty) {
        wordsSummary[row / 64] &= ~(summaryBit);
      }
    }
  }

  std::optional<size_t> findMaxTickIndex() const {
    for (int i = static_cast<int>(wordsSummary.size() - 1); i >= 0; --i) {
      if (wordsSummary[i] != 0) {
        size_t rowInBlock = std::bit_width(wordsSummary[i]) - 1;
        size_t row = (static_cast<size_t>(i) * 64) + rowInBlock;
        const auto &wordsRow = words[row];
        for (int j = wordsRow.size() - 1; j >= 0; --j) {
          if (uint64_t word = wordsRow[j]; word != 0) {
            int bitPos = std::bit_width(word) - 1;
            size_t index = (row * BITS_PER_ROW) + (j * BITS_PER_WORD) + bitPos;
            return index;
          }
        }
      }
    }
    return std::nullopt;
  }

  std::optional<size_t> getLowestTickIndex() const {
    for (size_t i = 0; i < wordsSummary.size(); ++i) {
      if (uint64_t summary = wordsSummary[i]; summary != 0) {
        uint32_t rowInBlock = static_cast<uint32_t>(std::countr_zero(summary));
        uint32_t row = (i * 64) + rowInBlock;
        const auto &wordsRow = words[row];
        for (size_t j = 0; j < wordsRow.size(); ++j) {
          if (uint64_t word = wordsRow[j]; word != 0) {
            int bitPos = std::countr_zero(word);
            size_t index = (row * BITS_PER_ROW) + (j * BITS_PER_WORD) + bitPos;
            return index;
          }
        }
      }
    }
    return std::nullopt;
  }
};
