#include "TemporaryDirectory.h"
#include "RandomString.h"
#include <string>
#include <catch2/catch.hpp>
#include <fstream>
#include <cstring>

namespace fs = std::filesystem;

TemporaryDirectory::TemporaryDirectory() : TemporaryDirectory(".tmp") {}

TemporaryDirectory::TemporaryDirectory(const std::string& fileExtension)
    : extension_(fileExtension), path_(createTempDir()) {}

const fs::path TemporaryDirectory::createTempDir() {
  const auto TempDir = fs::temp_directory_path();
  auto dir = fs::path(TempDir);
  while (exists(dir)) {
    dir = fs::path(TempDir).append(generateRandomString(8));
  }
  REQUIRE(create_directory(dir));
  return dir;
}

TemporaryDirectory::~TemporaryDirectory() {
  remove_all(path_);
}

 std::filesystem::path TemporaryDirectory::createTempFile() {
  auto outPath = fs::path(path_).append(generateRandomString(8) + extension_);
   auto stream = std::ofstream(outPath);
  stream.close();
  return outPath;
 }

 std::filesystem::path TemporaryDirectory::createNewFileInDir(
     std::string name) {
   auto outPath = fs::path(path_).append(name + extension_);
   auto stream = std::ofstream(outPath);
   stream.close();
   return outPath;
 }

 std::filesystem::path TemporaryDirectory::getPath() const { return path_; }

TEST_CASE("Temporary Directory created", "[utility][temporarydir]") {
  {
    auto tempDir = TemporaryDirectory();
    REQUIRE(fs::exists(tempDir.getPath()));
  }
}

TEST_CASE("Temporary File created", "[utility][temporarydir]") {
  {
    auto tempDir = TemporaryDirectory();
    auto tmpFile = tempDir.createTempFile();
    REQUIRE(fs::exists(tmpFile));
  }
}

TEST_CASE("Named Temporary File created", "[utility][temporarydir]") {
  {
    auto tempDir = TemporaryDirectory();
    auto tmpFile = tempDir.createNewFileInDir("testFile");
    REQUIRE(fs::exists(tmpFile));
    REQUIRE(tmpFile.filename() == "testFile.tmp");
  }
}
