#pragma once
#include "../include/CeresTask/Task.h"
#include <string>

class TestTask : public Task {
 public:
  TestTask(std::string name);
  virtual ~TestTask() = default;
  void run() override;
  const std::string& getName() const;

 private:
  const std::string name;
};