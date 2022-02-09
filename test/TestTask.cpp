#include "TestTask.h"

TestTask::TestTask() : Task() {}

TestTask::TestTask(std::string name) : Task(name) {}

bool TestTask::run(TaskID id) {
  auto lock = std::unique_lock<std::mutex>(taskMutex);
  ran = true;
  return true;
}

void TestTask::reset() {
  auto lock = std::unique_lock<std::mutex>(taskMutex);
  ran = false;
}

bool TestTask::hasRan() {
  auto lock = std::unique_lock<std::mutex>(taskMutex);
  return ran;
}
