#include "MappedFile.hpp"
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  fs::path filePath =
      "resources/data_feeds_20260402_20260402_IEXTP1_DEEP1.0.pcap.gz";
  MappedFile mappedFile(filePath);
  std::cout << "First 100 bytes: " << mappedFile.view().substr(0, 100) << '\n';
  // create order struct
  // create orders out of the data
}
