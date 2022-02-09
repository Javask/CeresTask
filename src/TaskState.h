#pragma once
#include <memory>
#include <mutex>
#include <vector>

#include "RegisteredTask.h"

struct TaskState {
  TaskState& operator=(const TaskState&) = delete;
  TaskState& operator=(TaskState&&);
  TaskState(const TaskState&) = delete;
  TaskState(TaskState&&);
  TaskState() = default;

  std::mutex tasksMutex;
  std::vector<RegisteredTask> tasks;
};