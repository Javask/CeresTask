#pragma once
#include <filesystem>
#include <string>

class TemporaryDirectory {
 public:
  TemporaryDirectory();
  TemporaryDirectory(const std::string& fileExtension);
  ~TemporaryDirectory();
  std::filesystem::path createTempFile();
  std::filesystem::path createNewFileInDir(std::string name);
  std::filesystem::path getPath() const;
 private:
  std::string extension_;
  std::filesystem::path path_;
  static const std::filesystem::path createTempDir();
};
