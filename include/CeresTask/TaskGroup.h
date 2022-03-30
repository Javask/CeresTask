#pragma once
#include "Task.h"
#include <memory>
#include <mutex>
#include <stack>
#include <map>
#include <set>
#include <cstdint>
#include <optional>
#include <shared_mutex>
/**
 * @brief Task group class representing a directed graph of tasks to be executed
 * Can be submitted to be asynchronously executed, respecting the constraints
 * created by the wait On dependency
 */
class TaskGroup {
  friend class Executor;

 public:
  /**
   * @brief Construct a new Task Group object
   * Sets the name to a random name
   */
  TaskGroup();

  /**
   * @brief Construct a new Task Group object
   * @param name Name of the Group for logging purposes
   */
  explicit TaskGroup(std::string name);

  /**
   * @brief Add a task to the Group
   * Will move the task into the Group
   * It can then be accessed by its id
   * By default it will be executed as soon as the group is set to be executed
   * @param task Task to be added to the group
   */
  void addTask(std::unique_ptr<Task> task);

  /**
   * @brief Remove a task from the group
   * @param id ID of the task to be removed
   * @returns std::unique_ptr<Task> to the task that was removed, or nullptr
   */
  std::unique_ptr<Task> removeTask(TaskID id);

  /**
   * @brief Get the task names of the tasks contained in the group
   * @return std::map<TaskID, std::string> of the ids with their names
   */
  std::map<TaskID, std::string> getContainedTaskNames();

  /**
   * @brief Get the task ids contained in the group
   * @return std::set<TaskID> of the task IDs
   */
  std::set<TaskID> getContainedTaskIDs();

  /**
   * @brief Add an edge to the wait dependency graph
   * Will add a dependency that will cause the task with waitingID to wait on
   * completion of the task with the ID waitOnID
   * @param waitOnID Task that should be waited on
   * @param waitingID  Task that should wait
   */
  void addWaitOnEdge(TaskID waitOnID, TaskID waitingID);

  /**
   * @brief Removes all tasks from the group
   */
  void clear();

  /**
   * @brief Get the Name of this Task Group
   *
   * @return const std::string&
   */
  const std::string& getName();

  /**
   * @brief Struct encapsulating the contents of the Task group
   *
   */
  struct Contents {
    std::vector<std::unique_ptr<Task>> tasks;
    std::map<TaskID, size_t> taskLookup;
    std::vector<std::pair<TaskID, TaskID>> waitOn;
  };
  /**
   * @brief Move the contents of the Group out of the object
   *
   * @return Contents of the Object
   */
  Contents moveContent();

 private:
  const std::string name;

  std::shared_mutex coherencyMutex;

  std::mutex freeTaskIndicesMutex;
  std::stack<size_t> freeTaskIndices;
  std::optional<size_t> getFreeIndex();
  void addFreeIndex(size_t index);

  std::mutex tasksMutex;
  std::vector<std::unique_ptr<Task>> tasks;
  size_t addTaskToVector(std::unique_ptr<Task> task);
  size_t addTaskToVector(std::unique_ptr<Task> task, size_t index);
  std::unique_ptr<Task> removeTaskFromVector(size_t index);

  std::mutex taskLookupMutex;
  std::map<TaskID, size_t> taskLookup;
  void addTaskToLookup(TaskID id, size_t index);
  std::optional<size_t> removeTaskFromLookup(TaskID id);

  std::mutex waitOnMutex;
  std::vector<std::pair<TaskID, TaskID>> waitOn;
  bool checkTasksAreInGroup(TaskID start, TaskID end);
  void addToWaitOn(TaskID start, TaskID end);
  void removeEdgeFromWaitOn(TaskID waitOn, TaskID waiting);
  void removeTaskFromWaitOn(TaskID id);
  void removeStartFromWaitOn(TaskID id);
  void removeEndFromWaitOn(TaskID id);
};