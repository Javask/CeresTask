#pragma once
#include "Task.h"
#include "TaskID.h"

#include <memory>


/// <summary>
/// Static class to interface with the library
/// </summary>
class TaskSystem {
 private:
  TaskSystem() = default;
  ~TaskSystem() = default;

  public:
  /// <summary>
  /// Give task to the System to be executed
  /// </summary>
  /// <param name="task">Task to be added to the system</param>
  /// <returns>Internal TaskID by which the task should be referenced from here on out</returns>
  static TaskID addTask(std::unique_ptr<Task> task);

  /// <summary>
  /// Replace a task with a given id with a new task, if id did not exist, create task with given id.
  /// </summary>
  /// <param name="id">Id to replace or create</param>
  /// <param name="newTask">Task to replace with, or set as the new Task</param>
  /// <returns>The replaced task, nullptr if new task</returns>
  static std::unique_ptr<Task> replaceTask(TaskID id, std::unique_ptr<Task> newTask);

  /// <summary>
  /// Remove a task with a given id from registered tasks.
  /// </summary>
  /// <param name="id">Id of task to be removed</param>
  /// <returns>The removed task, nullptr if not found</returns>
  static std::unique_ptr<Task> removeTask(TaskID id);
};