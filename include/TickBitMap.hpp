#include <array>
#include <bit>
#include <cassert>
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
  void setTick(const uint64_t tick, const bool value) {
    const size_t row = tick / BITS_PER_ROW;

    assert(row < NumRows && "Tick index is out of bounds");
    if ((row >= NumRows)) [[unlikely]] {
      return;
    }

    const size_t col = (tick % BITS_PER_ROW) / BITS_PER_WORD;
    const size_t pos = tick % BITS_PER_WORD;
    const uint64_t bit = (1ULL << pos);
    const uint64_t summaryBit = (1ULL << row % 64);

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

  std::optional<size_t> getMaxTickIndex() const {
    for (int i = static_cast<int>(wordsSummary.size()) - 1; i >= 0; --i) {
      if (wordsSummary[i] != 0) {
        size_t rowInBlock = 63 - std::countl_zero(wordsSummary[i]);
        size_t row = (static_cast<size_t>(i) * 64) + rowInBlock;
        const auto &wordsRow = words[row];
        for (int j = static_cast<int>(wordsRow.size()) - 1; j >= 0; --j) {
          if (uint64_t word = wordsRow[j]; word != 0) {
            int bitPos = 63 - std::countl_zero(word);
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

  bool test(const uint64_t tick) const {
    const size_t row = tick / BITS_PER_ROW;

    if (row >= NumRows) [[unlikely]] {
      return false;
    }

    const size_t col = (tick % BITS_PER_ROW) / BITS_PER_WORD;
    const size_t pos = tick % BITS_PER_WORD;

    return (words[row][col] & (1ULL << pos)) != 0;
  }
};
