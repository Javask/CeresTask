#pragma once
#include "../include/CeresTask/Task.h"
#include <string>
#include <mutex>

class TestTask : public Task {
 public:
  TestTask();
  TestTask(std::string name);
  virtual ~TestTask() = default;

  bool run(TaskID id) override;
  void reset();
  bool hasRan();

 private:
  std::mutex taskMutex;
  bool ran = false;
};