#include "../include/CeresTask/TaskID.h"
#include <catch2/catch.hpp>

TEST_CASE("TaskID tests", "[tasking][taskid]") {
  auto id = TaskID();
  auto id2 = TaskID();
  auto id1copy = id;
  auto id2copy = id2;
  SECTION("Compare task ids") {
    REQUIRE(id < id2);
    REQUIRE(!(id > id2));

    REQUIRE(id2 > id);
    REQUIRE(!(id2 < id));

    REQUIRE(id != id2);
    REQUIRE(id == id1copy);

    REQUIRE(id2 <= id2copy);
    REQUIRE(!(id2 <= id));
    REQUIRE(id <= id2);

    REQUIRE(id2 >= id2copy);
    REQUIRE(!(id >= id2));
    REQUIRE(id2 >= id);
  }
  SECTION("Compare strings") {
    REQUIRE(id.toString() == id1copy.toString());
    REQUIRE(id.toString() != id2.toString());
  }
  SECTION("Copies persist") {
    id = TaskID();
    REQUIRE(id != id1copy);
  }
}