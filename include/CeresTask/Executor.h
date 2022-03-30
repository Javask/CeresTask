#pragma once
#include "TaskGroup.h"
#include <thread>
#include <deque>
#include <atomic>
#include <shared_mutex>
#include <future>
#include <condition_variable>
#include <map>

/// <summary>
/// Executor class representing multiple threads that will execute submitted Task Groups, will join all threads on destruction and wait for each thread to finish current task
/// </summary>
class Executor {
 public:
  /// <summary>
  /// Create an Executor with an amount of threads to execute tasks
  /// </summary>
  /// <param name="threadCount">The number of threads to be started to execute
  /// tasks</param>
  explicit Executor(size_t threadCount = 1);
  ~Executor();

  /// <summary>
  /// Submit a TaskGroup for execution, adding it to a queue of waiting task
  /// groups to be executed simultaneously
  /// </summary>
  /// <param name="group"></param>
  /// <returns></returns>
  std::shared_future<bool> submit(std::unique_ptr<TaskGroup> group);
  std::shared_future<bool> submit(std::unique_ptr<Task> task);

 private:
  struct RegisteredTask {
    std::shared_ptr<Task> taskPtr;
    std::set<TaskID> afterThis;
    std::set<TaskID> beforeThisNotYetExecuted;
    bool oneBeforeHasFailed = false;
  };

  struct PromiseContainer {
    std::set<TaskID> waitingOn;
    bool hasFailed = false;
    std::promise<bool> promise;
  };

  static std::map<TaskID, RegisteredTask> registerTaskGroup(
      std::unique_ptr<TaskGroup> group);
  static std::set<TaskID> getEndTasks(const TaskGroup& groupRef);
  static std::set<TaskID> getStartTasks(const TaskGroup& groupRef);

  std::condition_variable newTasksReady;
  std::mutex readyTasksMutex;
  std::deque<TaskID> readyTasks;
  std::mutex registeredTasksMutex;
  std::map<TaskID, RegisteredTask> registeredTasks;
  std::mutex promisesMutex;

  std::vector<PromiseContainer> promises;

  std::mutex threadsMutex;
  std::vector<std::thread> threads;
  std::atomic_bool shouldRun = true;

  void threadMain(size_t id);
  std::optional<TaskID> threadGetTaskToRun();
  std::pair<std::shared_ptr<Task>,bool> threadFetchTaskInfoByID(TaskID id);
  void setTasksReady(std::set<TaskID> tasks);
  void threadHandleHasExecuted(TaskID id, bool result);
};