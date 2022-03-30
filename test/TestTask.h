#pragma once
#include "../include/CeresTask/Task.h"
#include <string>
#include <mutex>
#include <atomic>
#include <functional>

class TestTask : public Task {
 public:
  explicit TestTask();
  explicit TestTask(const std::string& name);

  ~TestTask() override = default;

  bool run(bool beforeHasFailed) override;
  void reset();
  bool hasRan();
  bool hasFailed();
  void onFail() override;
  void setShouldFail();
  void setShouldFailIfBeforeFailed();
  void setCustomRunFunction(std::function<bool(bool)> func);

 private:
  std::atomic_bool ran = false;
  std::atomic_bool failed = false;

  std::mutex callbackMutex;
  std::function<bool (bool)> callback;
};