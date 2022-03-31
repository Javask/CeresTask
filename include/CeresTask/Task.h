#pragma once
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
  explicit Task();

  /// <summary>
  /// Creates a Task
  /// </summary>
  /// <param name="name">Name of the task, will appear in logs</param>
  explicit Task(std::string name) ;

 public:
  virtual ~Task() = default;
  /// <summary>
  /// Returns the name of the task
  /// </summary>
  /// <returns>Name of the Task</returns>
  [[nodiscard]] const std::string& getName() const;

  /// <summary>
  /// Main function implementing the behaviour of the Task
  /// </summary>
  /// <param name="beforeHasFailed">Whether or not one of the directly preceding tasks failed execution.</param>
  /// <returns>Returns true after successful execution, false on
  /// failure</returns>
  virtual bool run(bool beforeHasFailed) = 0;

  /// <summary>
  /// Executed on fail of execution
  /// </summary>
  virtual void onFail();

  /// <summary>
  /// Returns the id of the task, initialized at creation to be runtime unique
  ///</summary>
  ///< returns>Id of this task</returns>
  [[nodiscard]] const TaskID& getID() const;

 private:
  const TaskID id;
  const std::string name;
};