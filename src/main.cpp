#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <sys/mman.h>
#include <unistd.h>

namespace fs = std::filesystem;

bool readFile(const char *filePath) {
  if (!fs::exists(filePath)) {
    std::cerr << "File not found: " << fs::absolute(filePath) << "\n";
    return false;
  }

  size_t fileSize = fs::file_size(filePath);
  int fd = open(filePath, O_RDONLY);
  if (fd == -1) {
    perror("Error opening file");
    return false;
  }

  void *mmappedData = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
  madvise(mmappedData, fileSize, MADV_SEQUENTIAL);
  std::string_view data(static_cast<char *>(mmappedData), fileSize);

  std::cout << "First 100 bytes: " << data.substr(0, 100) << std::endl;

  if (munmap(mmappedData, fileSize) != 0) {
    perror("munmap failed");
  }

  close(fd);
  return true;
}

int main(int argc, char **argv) {
  fs::path filePath =
      "resources/data_feeds_20260402_20260402_IEXTP1_DEEP1.0.pcap.gz";
  std::cout << readFile(filePath.c_str()) << " was the result";
}
