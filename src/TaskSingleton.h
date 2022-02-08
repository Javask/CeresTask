#pragma once
#include "Singleton.h"
#include "../include/CeresTask/TaskID.h"
#include "../include/CeresTask/Task.h"
#include "TaskState.h"
#include <shared_mutex>

/// <summary>
/// @see TaskSystem
/// Singleton implementing TaskSystems interface
/// </summary>
class TaskSingleton : public Singleton<TaskSingleton> {
  friend class Singleton<TaskSingleton>;
  friend class Destroyer<TaskSingleton>;

 protected:
  TaskSingleton() = default;
  ~TaskSingleton() = default;

 public:
  std::unique_ptr<Task> replaceTask(TaskID id, std::unique_ptr<Task> newTask);
  TaskID addTask(std::unique_ptr<Task> task);
  std::unique_ptr<Task> removeTask(TaskID id);

  /// <summary>
  /// Replace current Task state with new one, to inspect state
  /// </summary>
  /// <param name="newState">New State to be inserted</param>
  /// <returns>Old State to be inspected</returns>
  TaskState replaceState(TaskState newState);

 private:
  std::shared_mutex stateMutex;
  TaskState state;
};