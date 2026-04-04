#include "MappedFile.hpp"
#include <fcntl.h>
#include <string_view>
#include <sys/mman.h>
#include <unistd.h>

MappedFile::MappedFile(const fs::path &path) {
  fd_ = open(path.c_str(), O_RDONLY);

  if (fd_ == -1) {
    throw std::runtime_error("Could not open file");
  }
  size_ = fs::file_size(path);
  data_ = mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd_, 0);
  if (data_ == MAP_FAILED) {
    close(fd_);
    throw std::runtime_error("mmap failed");
  }
  madvise(data_, size_, MADV_SEQUENTIAL);
}

MappedFile::~MappedFile() {
  if (data_ != MAP_FAILED) {
    munmap(data_, size_);
  }

  if (fd_ != -1) {
    close(fd_);
  }
}

MappedFile &MappedFile::operator=(MappedFile &&other) noexcept {
  if (this != &other) {
    if (data_ != MAP_FAILED) {
      munmap(data_, size_);
    }

    if (fd_ != -1) {
      close(fd_);
    }
    fd_ = other.fd_;
    size_ = other.size_;
    data_ = other.data_;

    other.fd_ = -1;
    other.size_ = 0;
    other.data_ = nullptr;
  }

  return *this;
}

std::string_view MappedFile::view() {

  return {static_cast<char *>(data_), size_};
}
