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
  REQUIRE(state.tasks[taskID].getRawTaskPtr()->getName() == "test");
  REQUIRE(state.tasks[taskID2].getRawTaskPtr()->getName() == "test2");

  state = TaskSingleton::Get()->replaceState(std::move(state));
  auto deleted = TaskSystem::removeTask(taskID);
  REQUIRE(deleted);
  REQUIRE(deleted->getName() == "test");
  auto replaced = TaskSystem::replaceTask(taskID2, std::move(task3));
  REQUIRE(replaced);
  REQUIRE(replaced->getName() == "test2");

  
  state = TaskSingleton::Get()->replaceState(std::move(TaskState()));
  REQUIRE(state.tasks.size() == 2);
  REQUIRE(state.tasks[taskID].getRawTaskPtr() == nullptr);
  REQUIRE(state.tasks[taskID2].getRawTaskPtr());
  REQUIRE(state.tasks[taskID2].getRawTaskPtr()->getName() == "test3");
}

TEST_CASE("Check task execution", "[tasking]") {
  auto task = std::make_unique<TestTask>("test");
  auto id = TaskSystem::addTask(std::move(task));
  TaskSystem::setRunnable(id, true);
  REQUIRE(TaskSystem::waitOnTaskCompletion(id,1000));
  REQUIRE(TaskSystem::checkHasRun(id));
  auto returnedTask = TaskSystem::removeTask(id);
  REQUIRE(returnedTask);
  REQUIRE(((TestTask*)returnedTask.get())->hasRan());
}

TEST_CASE("Check task dependencies", "[tasking]") {
  auto task = std::make_unique<TestTask>("test");
  auto task2 = std::make_unique<TestTask>("test2");
  auto taskID = TaskSystem::addTask(std::move(task));
  auto taskID2 = TaskSystem::addTask(std::move(task2));
  REQUIRE(TaskSystem::setWaitingOn(taskID2, taskID));
  TaskSystem::setRunnable(taskID2);
  REQUIRE(!TaskSystem::checkIsReady(taskID2));
  REQUIRE(TaskSystem::checkIsWaiting(taskID2));
  REQUIRE(!TaskSystem::checkIsReady(taskID));
  REQUIRE(!TaskSystem::checkIsWaiting(taskID));
  TaskSystem::setRunnable(taskID);
  REQUIRE(TaskSystem::checkIsReady(taskID));
  REQUIRE(!TaskSystem::checkIsWaiting(taskID));
  REQUIRE(TaskSystem::waitOnTaskCompletion(taskID,1000));
  REQUIRE(TaskSystem::checkIsReady(taskID2));
  REQUIRE(!TaskSystem::checkIsWaiting(taskID2));
  REQUIRE(TaskSystem::waitOnTaskCompletion(taskID2,1000));
}