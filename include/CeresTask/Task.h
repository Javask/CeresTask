#pragma once
#include <string>
#include <vector>
#include "TaskID.h"

/// <summary>
/// Virtual class that should be inherited from to create an executable task
/// </summary>
class Task {
 protected:
  /// <summary>
  /// Creates a Task with a random name
  /// </summary>
  /// <param name="runOnce">Whether or not the task should disable after running
  /// (default: false)</param>
  Task(bool runOnce = false);

  /// <summary>
  /// Creates a Task
  /// </summary>
  /// <param name="name">Name of the task, will appear in logs</param>
  /// <param name="runOnce">Whether or not the task should disable after running
  /// (default: false)</param>
  Task(const std::string& name, bool runOnce = false);



 public:
  virtual ~Task() = default;
  /// <summary>
  /// Returns the name of the task
  /// </summary>
  /// <returns>Name of the Task</returns>
  const std::string& getName() const;

  /// <summary>
  /// Main function implementing the behaviour of the Task
  /// </summary>
  /// <param name="id">id with which the task is being ran</param>
  /// <returns>Returns true after successfull execution, false on
  /// failure</returns>
  virtual bool run(TaskID id) = 0;

 private:
  const std::string name;
  const bool runOnce;
};