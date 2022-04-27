#include "../include/CeresTask/Executor.h"
#include <CeresLog/Logging.h>

namespace CeresTask {

Executor::Executor(size_t threadCount) {
  auto lock = std::unique_lock(threadsMutex);
  threads.reserve(threadCount);
  for (size_t i = 0; i < threadCount; i++) {
    auto lambda = [this, i]() { threadMain(i); };
    threads.emplace_back(lambda);
  }
}

Executor::~Executor() {
  shouldRun.store(false);
  auto lock = std::unique_lock(threadsMutex);
  Debug("[Executor] Joining Threads...");
  for (auto& thread : threads) {
    thread.join();
  }
}

void Executor::threadMain(size_t id) {
  Debug("[Executor] Thread " + std::to_string(id) + "started.");
  while (shouldRun) {
    auto toRun = threadGetTaskToRun();
    if (toRun.has_value()) {
      auto taskInfo = threadFetchTaskInfoByID(*toRun);
      if (taskInfo.first) {
        auto result = taskInfo.first->run(taskInfo.second);
        if (!result) {
          taskInfo.first->onFail();
        }
        threadHandleHasExecuted(*toRun, result);
      }
    }
  }
  Debug("[Executor] Thread " + std::to_string(id) + "stopped.");
}

std::optional<TaskID> Executor::threadGetTaskToRun() {
  auto readyTasksLock = std::unique_lock(readyTasksMutex);
  if (readyTasks.empty()) {
    newTasksReady.wait_for(readyTasksLock, std::chrono::milliseconds(100));
  }
  if (!readyTasks.empty()) {
    auto out = readyTasks.front();
    readyTasks.pop_front();
    return out;
  }
  return {};
}

std::pair<std::shared_ptr<Task>, bool> Executor::threadFetchTaskInfoByID(
    TaskID id) {
  auto registeredTaskLock = std::unique_lock(registeredTasksMutex);
  if (!registeredTasks.contains(id)) {
    return {nullptr, false};
  }
  return {registeredTasks[id].taskPtr, registeredTasks[id].oneBeforeHasFailed};
}

void Executor::setTasksReady(std::set<TaskID> tasks) {
  {
    auto readyTasksLock = std::unique_lock(readyTasksMutex);
    for (auto task : tasks) {
      readyTasks.push_back(task);
    }
  }
  if (tasks.size() == 1) {
    newTasksReady.notify_one();
  } else if (tasks.size() >= 1) {
    newTasksReady.notify_all();
  }
}

void Executor::threadHandleHasExecuted(TaskID id, bool result) {
  auto setReady = std::set<TaskID>();
  {
    auto registeredTaskLock = std::unique_lock(registeredTasksMutex);
    if (!registeredTasks.contains(id)) return;
    auto afterThis = registeredTasks[id].afterThis;
    registeredTasks.erase(id);

    for (auto afterId : afterThis) {
      if (registeredTasks.contains(afterId)) {
        registeredTasks[afterId].beforeThisNotYetExecuted.erase(id);
        if (!result) {
          registeredTasks[afterId].oneBeforeHasFailed = true;
        }
        if (registeredTasks[afterId].beforeThisNotYetExecuted.empty()) {
          setReady.insert(afterId);
        }
      }
    }
  }
  setTasksReady(setReady);
  {
    auto promisesLock = std::unique_lock(promisesMutex);
    for (auto it = promises.begin(); it != promises.end(); it++) {
      if (it->waitingOn.contains(id)) {
        it->waitingOn.erase(id);
        if (!result) {
          it->hasFailed = true;
        }
        if (it->waitingOn.empty()) {
          it->promise.set_value(!it->hasFailed);
          it = promises.erase(it);
          if (it == promises.end()) break;
        }
      }
    }
  }
}

std::shared_future<bool> Executor::submit(std::unique_ptr<Task> task) {
  auto taskGroup = std::make_unique<TaskGroup>();
  taskGroup->addTask(std::move(task));
  return submit(std::move(taskGroup));
}

std::map<TaskID, Executor::RegisteredTask> Executor::registerTaskGroup(
    std::unique_ptr<TaskGroup> group) {
  auto outMap = std::map<TaskID, RegisteredTask>();
  for (auto& taskRef : group->tasks) {
    auto currID = taskRef->getID();
    auto regTask = RegisteredTask();
    regTask.taskPtr = std::move(taskRef);
    for (auto& waitOnPair : group->waitOn) {
      if (waitOnPair.first == currID) {
        regTask.afterThis.insert(waitOnPair.second);
      }
      if (waitOnPair.second == currID) {
        regTask.beforeThisNotYetExecuted.insert(waitOnPair.first);
      }
    }
    outMap.emplace(currID, std::move(regTask));
  }
  return outMap;
}

std::set<TaskID> Executor::getEndTasks(const TaskGroup& groupRef) {
  auto outSet = std::set<TaskID>();
  for (auto& taskRef : groupRef.tasks) {
    auto currID = taskRef->getID();
    auto endTask = true;
    for (auto& waitOnRef : groupRef.waitOn) {
      if (waitOnRef.first == currID) {
        endTask = false;
        break;
      }
    }
    if (endTask) {
      outSet.insert(currID);
    }
  }
  return outSet;
}

std::set<TaskID> Executor::getStartTasks(const TaskGroup& groupRef) {
  auto outSet = std::set<TaskID>();
  for (auto& taskRef : groupRef.tasks) {
    auto currID = taskRef->getID();
    auto startTask = true;
    for (auto& waitOnRef : groupRef.waitOn) {
      if (waitOnRef.second == currID) {
        startTask = false;
        break;
      }
    }
    if (startTask) {
      outSet.insert(currID);
    }
  }
  return outSet;
}

std::shared_future<bool> Executor::submit(std::unique_ptr<TaskGroup> group) {
  auto container = PromiseContainer();
  auto future = std::shared_future(container.promise.get_future());
  auto startTasks = Executor::getStartTasks(*group);
  container.waitingOn = Executor::getEndTasks(*group);
  auto map = Executor::registerTaskGroup(std::move(group));
  {
    auto promisesLock = std::unique_lock(promisesMutex);
    promises.push_back(std::move(container));
  }
  {
    auto registeredTasksLock = std::unique_lock(registeredTasksMutex);
    registeredTasks.merge(map);
  }
  if (startTasks.empty()) {
    Error("TaskGroup has no ready tasks!");
  }
  setTasksReady(startTasks);
  return future;
}

}  // namespace CeresTask