#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../include/CeresTask/Executor.h"
#include "../include/CeresTask/TaskGroup.h"
#include "../include/CeresTask/Task.h"
#include "TestTask.h"

TEST_CASE("Single task executes correctly", "[tasking][usage]") {
  auto exec = Executor(1);
  auto taskGroup = std::make_unique<TaskGroup>("simpleTasks");
  auto testTask = std::make_unique<TestTask>("simpleTestTask1");
  SECTION("Check successfull tasks") {
    taskGroup->addTask(std::move(testTask));
    auto future = exec.submit(std::move(taskGroup));
    auto done = future.wait_for(std::chrono::milliseconds(1000));
    REQUIRE(done == std::future_status::ready);
    REQUIRE(future.valid());
    REQUIRE(future.get());
  }
  SECTION("Check failing tasks") {
    testTask->setShouldFail();
    taskGroup->addTask(std::move(testTask));
    auto future = exec.submit(std::move(taskGroup));
    auto done = future.wait_for(std::chrono::milliseconds(1000));
    REQUIRE(done == std::future_status::ready);
    REQUIRE(future.valid());
    REQUIRE(!future.get());
  }
}

TEST_CASE("Two tasks behave correctly", "[tasking][usage]") {
  auto exec = Executor(1);
  auto taskGroup = std::make_unique<TaskGroup>("simpleTasks");
  auto testTask1 = std::make_unique<TestTask>("simpleTestTask1");
  auto testTask2 = std::make_unique<TestTask>("simpleTestTask2");
  auto id1 = testTask1->getID();
  auto id2 = testTask2->getID();

  SECTION("Completely parallel both successfull") {
    taskGroup->addTask(std::move(testTask1));
    taskGroup->addTask(std::move(testTask2));
    auto future = exec.submit(std::move(taskGroup));
    auto done = future.wait_for(std::chrono::milliseconds(1000));
    REQUIRE(done == std::future_status::ready);
    REQUIRE(future.valid());
    REQUIRE(future.get());
  }
  SECTION("Completely parallel one fails") {
    testTask2->setShouldFail();
    taskGroup->addTask(std::move(testTask1));
    taskGroup->addTask(std::move(testTask2));
    auto future = exec.submit(std::move(taskGroup));
    auto done = future.wait_for(std::chrono::milliseconds(1000));
    REQUIRE(done == std::future_status::ready);
    REQUIRE(future.valid());
    REQUIRE(!future.get());
  }

  SECTION("Sequential both successfull") {
    taskGroup->addTask(std::move(testTask1));
    taskGroup->addTask(std::move(testTask2));
    taskGroup->addWaitOnEdge(id1, id2);
    auto future = exec.submit(std::move(taskGroup));
    auto done = future.wait_for(std::chrono::milliseconds(1000));
    REQUIRE(done == std::future_status::ready);
    REQUIRE(future.valid());
    REQUIRE(future.get());
  }

  SECTION("Sequential first fails") {
    testTask1->setShouldFail();
    taskGroup->addTask(std::move(testTask1));
    taskGroup->addTask(std::move(testTask2));
    taskGroup->addWaitOnEdge(id1, id2);
    auto future = exec.submit(std::move(taskGroup));
    auto done = future.wait_for(std::chrono::milliseconds(1000));
    REQUIRE(done == std::future_status::ready);
    REQUIRE(future.valid());
    REQUIRE(future.get());
  }

  SECTION("Sequential second fails") {
    testTask2->setShouldFail();
    taskGroup->addTask(std::move(testTask1));
    taskGroup->addTask(std::move(testTask2));
    taskGroup->addWaitOnEdge(id1, id2);
    auto future = exec.submit(std::move(taskGroup));
    auto done = future.wait_for(std::chrono::milliseconds(1000));
    REQUIRE(done == std::future_status::ready);
    REQUIRE(future.valid());
    REQUIRE(!future.get());
  }

  SECTION("Sequential failure ripples through") {
    testTask1->setShouldFail();
    testTask2->setShouldFailIfBeforeFailed();
    taskGroup->addTask(std::move(testTask1));
    taskGroup->addTask(std::move(testTask2));
    taskGroup->addWaitOnEdge(id1, id2);
    auto future = exec.submit(std::move(taskGroup));
    auto done = future.wait_for(std::chrono::milliseconds(1000));
    REQUIRE(done == std::future_status::ready);
    REQUIRE(future.valid());
    REQUIRE(!future.get());
  }
}

TEST_CASE("Wait on works correctly", "[tasking][usage]") {
  auto exec = Executor(1);
  auto taskGroup = std::make_unique<TaskGroup>("interlockingTasks");
  auto testTask1 = std::make_unique<TestTask>("simpleTestTask1");
  auto testTask2 = std::make_unique<TestTask>("simpleTestTask2");
  auto testTask3 = std::make_unique<TestTask>("simpleTestTask3");
  auto testTask4 = std::make_unique<TestTask>("simpleTestTask4");
  auto testTask5 = std::make_unique<TestTask>("simpleTestTask5");
  auto testTask6 = std::make_unique<TestTask>("simpleTestTask6");
  std::atomic_uint stage1 = 0, stage2 = 0, stage3 = 0;

  auto id1 = testTask1->getID();
  auto id2 = testTask2->getID();
  auto id3 = testTask3->getID();
  auto id4 = testTask4->getID();
  auto id5 = testTask5->getID();
  auto id6 = testTask6->getID();

  auto stage1Func = [&stage1,&stage2,&stage3](bool) {
    stage1++;
    return stage2 == 0 && stage3 == 0;
  };
  auto stage2Func = [&stage1, &stage2, &stage3](bool before) {
    stage2++;
    return (stage1 == 1 && stage3 == 0) || before;
  };
  auto stage3Func = [&stage1, &stage2, &stage3](bool before) {
    stage3++;
    return (stage1 == 1 && stage2 == 2) || before;
  };
  auto stage4Func = [&stage1, &stage2, &stage3](bool before) {
    return (stage1 == 1 && stage2 == 2 && stage3 == 2) || before;
  };

  /* Dependency Graph:
       1      Stage1
      / \     ------
     2   3    Stage2
     |\ /|
     | X |    ------
     |/ \|    
     4   5    Stage3
      \ /     ------
       6      Stage4
  */

  testTask1->setCustomRunFunction(stage1Func);
  taskGroup->addTask(std::move(testTask1));
  testTask2->setCustomRunFunction(stage2Func);
  taskGroup->addTask(std::move(testTask2));
  testTask3->setCustomRunFunction(stage2Func);
  taskGroup->addTask(std::move(testTask3));
  testTask4->setCustomRunFunction(stage3Func);
  taskGroup->addTask(std::move(testTask4));
  testTask5->setCustomRunFunction(stage3Func);
  taskGroup->addTask(std::move(testTask5));
  testTask6->setCustomRunFunction(stage4Func);
  taskGroup->addTask(std::move(testTask6));
  taskGroup->addWaitOnEdge(id1, id2);
  taskGroup->addWaitOnEdge(id1, id3);
  taskGroup->addWaitOnEdge(id2, id4);
  taskGroup->addWaitOnEdge(id2, id5);
  taskGroup->addWaitOnEdge(id3, id4);
  taskGroup->addWaitOnEdge(id3, id5);
  taskGroup->addWaitOnEdge(id4, id6);
  taskGroup->addWaitOnEdge(id5, id6);

  auto future = exec.submit(std::move(taskGroup));
  auto done = future.wait_for(std::chrono::milliseconds(1000));
  REQUIRE(done == std::future_status::ready);
  REQUIRE(future.valid());
  REQUIRE(future.get());
  REQUIRE(stage1 == 1);
  REQUIRE(stage2 == 2);
  REQUIRE(stage3 == 2);
}