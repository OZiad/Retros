#include "IEXParser.hpp"
#include "MappedFile.hpp"
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <unistd.h>

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  fs::path filePath = "resources/clean_data.pcap";
  MappedFile mappedFile(filePath);
  iex::parser::parseIEX(
      mappedFile.view()); // TODO: let producer thread handle this

  // TODO: consumer thread, create new OrderBook every time a new symbol is
  // encountered (lazy loading basically)
}
