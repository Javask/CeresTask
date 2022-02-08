#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../include/CeresTask/TaskSystem.h"
#include "TestTask.h"
#include "../src/TaskSingleton.h"

TEST_CASE("Add/remove/replace task test", "[tasking]") {
  auto task = std::make_unique<TestTask>("test");
  auto task2 = std::make_unique<TestTask>("test2");
  auto task3 = std::make_unique<TestTask>("test3");

  auto taskID = TaskSystem::addTask(std::move(task));
  auto taskID2 = TaskSystem::addTask(std::move(task2));
  auto state = TaskSingleton::Get()->replaceState(std::move(TaskState()));
  REQUIRE(state.tasks.size() == 2);
  REQUIRE(((TestTask*)state.tasks[taskID].get())->getName() == "test");
  REQUIRE(((TestTask*)state.tasks[taskID2].get())->getName() == "test2");

  state = TaskSingleton::Get()->replaceState(std::move(state));
  auto deleted = TaskSystem::removeTask(taskID);
  REQUIRE(deleted);
  REQUIRE(((TestTask*)deleted.get())->getName() == "test");
  auto replaced = TaskSystem::replaceTask(taskID2, std::move(task3));
  REQUIRE(replaced);
  REQUIRE(((TestTask*)replaced.get())->getName() == "test2");

  
  state = TaskSingleton::Get()->replaceState(std::move(TaskState()));
  REQUIRE(state.tasks.size() == 2);
  REQUIRE(state.tasks[taskID] == nullptr);
  REQUIRE(((TestTask*)state.tasks[taskID2].get())->getName() == "test3");
}