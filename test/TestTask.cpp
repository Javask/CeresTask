#include "TestTask.h"
#include <thread>
#include <chrono>
#include <utility>

TestTask::TestTask() : Task() {}

TestTask::TestTask(const std::string& name) : Task(name) {}


bool TestTask::run(bool beforeHasFailed) {
  auto lock = std::unique_lock<std::mutex>(callbackMutex);
  ran.store(true);
  if (callback) return callback(beforeHasFailed);
  return true;
}

void TestTask::reset() {
  ran.store(false);
  failed.store(false);
}

bool TestTask::hasRan() { return ran.load(); }

void TestTask::setShouldFail() {
  auto lock = std::unique_lock<std::mutex>(callbackMutex);
  callback = [](bool) { return false; };
}

void TestTask::onFail() { failed.store(true); }

bool TestTask::hasFailed() { return failed.load(); }

void TestTask::setShouldFailIfBeforeFailed() {
  auto lock = std::unique_lock<std::mutex>(callbackMutex);
  callback = [](bool before) { return !before; };
}

void TestTask::setCustomRunFunction(std::function<bool( bool )> func) {
  auto lock = std::unique_lock<std::mutex>(callbackMutex);
  callback = std::move(func);
}