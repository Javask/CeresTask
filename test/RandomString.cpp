#include "RandomString.h"
#include <random>
#include <algorithm>
#include <catch2/catch.hpp>

auto generateRandomString(const std::size_t len) -> std::string {
  static constexpr auto chars =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  thread_local auto rng = std::random_device();
  auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
  auto result = std::string(len, '\0');
  std::generate_n(begin(result), len, [&]() { return chars[dist(rng)]; });
  return result;
}

TEST_CASE("Generated Random String", "[utility]") {
  auto str = generateRandomString(8);
  REQUIRE(str.size() == 8);
}
