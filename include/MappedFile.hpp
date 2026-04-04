#include <cstddef>
#include <filesystem>
#include <string_view>

namespace fs = std::filesystem;
class MappedFile {
public:
  explicit MappedFile(const fs::path &path);
  MappedFile(const MappedFile &) = delete;
  MappedFile &operator=(const MappedFile) = delete;
  MappedFile &operator=(MappedFile &&other) noexcept;
  ~MappedFile();
  std::string_view view();

private:
  int fd_ = -1;
  size_t size_;
  void *data_;
};
