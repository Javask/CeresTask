#pragma once
#include <memory>
#include <mutex>
#include <vector>

#include "../include/CeresTask/Task.h"

struct TaskState {
  TaskState& operator=(const TaskState&) = delete;
  TaskState& operator=(TaskState&&);
  TaskState(const TaskState&) = delete;
  TaskState(TaskState&&);
  TaskState() = default;

  std::mutex tasksMutex;
  std::vector<std::unique_ptr<Task>> tasks;
};