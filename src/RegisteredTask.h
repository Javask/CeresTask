#pragma once
#include "../include/CeresTask/Task.h"
#include <memory>
#include <atomic>
#include <shared_mutex>

/// <summary>
/// Wrapper class that has all the scheduling and queueing information for a task
/// </summary>
class RegisteredTask {
 public:
  /// <summary>
  /// Create a RegisteredTask container pointing to task
  /// </summary>
  /// <param name="task">Task to point to, if nullptr = empty</param>
  RegisteredTask(std::unique_ptr<Task> task = nullptr);
  ~RegisteredTask() = default;

  RegisteredTask& operator=(const RegisteredTask&) = delete;
  RegisteredTask& operator=(RegisteredTask&&) noexcept;
  RegisteredTask(const RegisteredTask&) = delete;
  RegisteredTask(RegisteredTask&&) noexcept;

  /// <summary>
  /// Replace the task this registred Task points to
  /// </summary>
  /// <param name="newTask">New task to point to</param>
  /// <returns>Old task that was pointed to</returns>
  std::unique_ptr<Task> replaceTask(std::unique_ptr<Task> newTask);

  /// <summary>
  /// Return the raw pointer to the Task registered here
  /// </summary>
  /// <returns>Raw Ptr of taskPtr</returns>
  Task* getRawTaskPtr();

  /// <summary>
  /// Adds a Task to the tasks this will wait on before becoming runnable.
  /// </summary>
  /// <param name="id">Id of the task to add</param>
  void addToWaitOn(TaskID id);

  /// <summary>
  /// Adds a number of Tasks to the tasks thiswill wait on before becoming
  /// runnable.
  /// </summary>
  /// <param name="ids">ids of the tasks to add</param>
  void addToWaitOn(std::vector<TaskID> ids);

  /// <summary>
  /// Adds a Task to the tasks this will notify after successfull execution.
  /// </summary>
  /// <param name="id">Id of the task to add</param>
  void addToNotify(TaskID id);

  /// <summary>
  /// Adds a number of Tasks to the tasks this will notify after successfull
  /// execution.
  /// </summary>
  /// <param name="ids">ids of the tasks to add</param>
  void addToNotify(std::vector<TaskID> ids);

  /// <summary>
  /// Called by task after running, notifying all other tasks waiting on it.
  /// </summary>
  /// <param name="hasRun">Id of the Task that ran</param>
  void setTaskHasRun(TaskID hasRun);

  /// <summary>
  /// Called by the TaskSystem, executing this and notifying other tasks on
  /// completion
  /// </summary>
  /// <param name="id"></param>
  /// <returns></returns>
  bool executeWithID(TaskID id);

  /// <summary>
  /// Clears the Tasks this waits on
  /// </summary>
  void clearWaitingOn();

  /// <summary>
  /// Removes a task from the tasks this will notify
  /// </summary>
  /// <param name="id">Task to remove</param>
  void removeToNotify(TaskID id);

  /// <summary>
  /// Get the tasks this waits on
  /// </summary>
  /// <returns>Array of task ids to wait on</returns>
  const std::vector<TaskID> getWaitingOn();

  /// <summary>
  /// Returns an array of bools representing whether or not the task with th id
  /// at the same index in getWaitingOn() has notified this task.
  /// </summary>
  /// <returns>Array of bools corresponding to getWaitingOn()s Ids</returns>
  const std::vector<bool> getHasWaitedOn();

  /// <summary>
  /// Array of Task Ids to notify after this task has successfully run
  /// </summary>
  /// <returns>Array of Task Ids to notify after this task has successfully
  /// run</returns>
  const std::vector<TaskID> getToNotify();

 private:
  std::shared_mutex taskMutex;
  std::vector<TaskID> waitOn = {};
  std::vector<bool> hasWaitedOn = {};
  std::vector<TaskID> toNotify = {};
  std::unique_ptr<Task> taskPtr;
};