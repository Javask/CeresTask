#pragma once
#include "Task.h"
#include "TaskID.h"

#include <memory>
#include <future>


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
  /// <returns>Internal TaskID by which the task should be referenced from here
  /// on out</returns>
  static TaskID addTask(std::unique_ptr<Task> task);

  /// <summary>
  /// Replace a task with a given id with a new task, if id did not exist,
  /// create task with given id.
  /// </summary>
  /// <param name="id">Id to replace or create</param>
  /// <param name="newTask">Task to replace with, or set as the new Task</param>
  /// <returns>The replaced task, nullptr if new task</returns>
  static std::unique_ptr<Task> replaceTask(TaskID id,
                                           std::unique_ptr<Task> newTask);

  /// <summary>
  /// Remove a task with a given id from registered tasks.
  /// </summary>
  /// <param name="id">Id of task to be removed</param>
  /// <returns>The removed task, nullptr if not found</returns>
  static std::unique_ptr<Task> removeTask(TaskID id);

  /// <summary>
  /// Set whether a Task should be able to be executed
  /// </summary>
  /// <param name="id">TaskID to modify</param>
  /// <param name="runnable">Whether or not the Task should be runnable
  /// (default: true)</param>
  static void setRunnable(TaskID id, bool runnable = true);

  /// <summary>
  /// Add a dependency between two tasks, with waitingTask waiting on toWaitOn
  /// successfully executing before being able to be scheduled
  /// </summary>
  /// <param name="waitingTask">Task Id of the task which should wait</param>
  /// <param name="toWaitOn">Task Id of the task to wait on</param>
  /// <returns>true if successfully set the dependency, will fail if waiting
  /// Task is running or waitingTask doesnt exist</returns>
  static bool setWaitingOn(TaskID waitingTask, TaskID toWaitOn);

  /// <summary>
  /// Check if a task has been run
  /// </summary>
  /// <param name="id">id of the task to check</param>
  /// <returns>true if successfully waited</returns>
  static bool checkHasRun(TaskID id);

  /// <summary>
  /// Check if a task is ready to be run 
  /// </summary>
  /// <param name="id">id of the task to check</param>
  /// <returns>true if task is runnable and not waiting</returns>
  static bool checkIsReady(TaskID id);

  /// <summary>
  /// Check if a task is waiting on another, ignoring whether it is runnable
  /// </summary>
  /// <param name="id">id of the task to check</param>
  /// <returns>true if task is waiting, ignoring whether it is runnable</returns>
  static bool checkIsWaiting(TaskID id);

  /// <summary>
  /// Waits on task completion
  /// </summary>
  /// <param name="id">Id of the task to wait on, returns if task doesnt exist</param>
  /// <param name="timeout">timeout in milliseconds, 0 is no timeout, waiting forever</param>
  /// <returns>true if successfully waited</returns>
  static bool waitOnTaskCompletion(TaskID id, size_t timeout = 0);
};