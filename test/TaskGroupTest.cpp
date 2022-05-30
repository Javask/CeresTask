#include "../include/CeresTask/TaskGroup.h"
#include <catch2/catch_test_macros.hpp>
#include "TestTask.h"

using namespace CeresTask;

template <typename T>
bool vectorContains(const std::vector<T>& vec, const T& obj) {
  for (auto& i : vec) {
    if (i == obj) return true;
  }
  return false;
}

TEST_CASE("Task Group tests", "[tasking][taskgroup]") {
  auto group = TaskGroup("test");
  SECTION("Name set correctly") { REQUIRE(group.getName() == "test"); }
  auto task1 = std::make_unique<TestTask>("task1");
  auto task1ID = task1->getID();
  auto task1Name = task1->getName();
  group.addTask(std::move(task1));

  auto task2 = std::make_unique<TestTask>("task2");
  auto task2ID = task2->getID();
  auto task2Name = task2->getName();
  group.addTask(std::move(task2));

  SECTION("Tasks added correctly") {
    auto contents = group.moveContent();
    REQUIRE(contents.taskLookup.size() == 2);
    REQUIRE(contents.taskLookup.contains(task1ID));
    REQUIRE(contents.taskLookup.contains(task2ID));

    auto task1Index = contents.taskLookup[task1ID];
    REQUIRE(task1Index < contents.tasks.size());
    REQUIRE(contents.tasks[task1Index]);
    REQUIRE(contents.tasks[task1Index]->getID() == task1ID);
    REQUIRE(contents.tasks[task1Index]->getName() == task1Name);

    auto task2Index = contents.taskLookup[task2ID];
    REQUIRE(task2Index < contents.tasks.size());
    REQUIRE(contents.tasks[task2Index]);
    REQUIRE(contents.tasks[task2Index]->getID() == task2ID);
    REQUIRE(contents.tasks[task2Index]->getName() == task2Name);

    REQUIRE(contents.waitOn.empty());
  }

  SECTION("Add third task with wait dependency") {
    auto task3 = std::make_unique<TestTask>("task3");
    auto task3ID = task3->getID();
    auto task3Name = task3->getName();
    group.addTask(std::move(task3));

    group.addWaitOnEdge(task1ID, task2ID);
    group.addWaitOnEdge(task2ID, task3ID);
    group.addWaitOnEdge(task1ID, task3ID);

    SECTION("Check waiting connections correct") {
      auto contents = group.moveContent();
      REQUIRE(contents.waitOn.size() == 3);
      REQUIRE(
          vectorContains(contents.waitOn, std::make_pair(task1ID, task2ID)));
      REQUIRE(
          vectorContains(contents.waitOn, std::make_pair(task2ID, task3ID)));
      REQUIRE(
          vectorContains(contents.waitOn, std::make_pair(task1ID, task3ID)));
    }
    SECTION("Remove task") {
      auto removedTask2 = group.removeTask(task2ID);

      SECTION("Double remove task") {
        auto tryDoubleRemove = group.removeTask(task2ID);
        REQUIRE(tryDoubleRemove == nullptr);
      }

      SECTION("Removed task correctly") {
        REQUIRE(removedTask2->getID() == task2ID);
        REQUIRE(removedTask2->getName() == task2Name);
      }

      SECTION("Waiting dependency correct after remove") {
        auto contents = group.moveContent();
        REQUIRE(contents.waitOn.size() == 1);
        REQUIRE(
            vectorContains(contents.waitOn, std::make_pair(task1ID, task3ID)));
      }
      SECTION("Lookup correct after remove") {
        auto contents = group.moveContent();
        REQUIRE(contents.taskLookup.size() == 2);
        REQUIRE(contents.taskLookup.contains(task1ID));
        REQUIRE(contents.taskLookup.contains(task2ID) == false);
        REQUIRE(contents.taskLookup.contains(task3ID));
      }

      SECTION("Check contained Ids and names correct") {
        auto containedIDs = group.getContainedTaskIDs();
        auto containedNames = group.getContainedTaskNames();
        REQUIRE(containedIDs.size() == 2);
        REQUIRE(containedNames.size() == 2);
        REQUIRE(containedIDs.contains(task1ID));
        REQUIRE(containedIDs.contains(task3ID));
        REQUIRE(containedNames.contains(task1ID));
        REQUIRE(containedNames[task1ID] == task1Name);
        REQUIRE(containedNames.contains(task3ID));
        REQUIRE(containedNames[task3ID] == task3Name);
      }
    }
  }
}