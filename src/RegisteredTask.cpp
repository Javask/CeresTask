#include "RegisteredTask.h"
#include "TaskSingleton.h"
#include <CeresLog/Logging.h>

RegisteredTask::RegisteredTask(std::unique_ptr<Task> task) : taskPtr(std::move(task)) {}

RegisteredTask& RegisteredTask::operator=(RegisteredTask&& other) noexcept {
  this->taskPtr = std::move(other.taskPtr);
  this->toNotify = std::move(other.toNotify);
  this->waitOn = std::move(other.waitOn);
  this->hasWaitedOn = std::move(other.hasWaitedOn);
  return *this;
}

RegisteredTask::RegisteredTask(RegisteredTask&& other) noexcept {
  this->taskPtr = std::move(other.taskPtr);
  this->toNotify = std::move(other.toNotify);
  this->waitOn = std::move(other.waitOn);
  this->hasWaitedOn = std::move(other.hasWaitedOn);
}

std::unique_ptr<Task> RegisteredTask::replaceTask(
    std::unique_ptr<Task> newTask) {
  std::unique_ptr<Task> out = std::move(taskPtr);
  taskPtr = std::move(newTask);
  return out;
}

Task* RegisteredTask::getRawTaskPtr() { return taskPtr.get(); }

void RegisteredTask::addToWaitOn(TaskID id) {
  std::unique_lock<std::shared_mutex> Lock(taskMutex);
  for (auto waitId : waitOn) {
    if (waitId == id) return;
  }
  waitOn.push_back(id);
  hasWaitedOn.push_back(false);
}

void RegisteredTask::addToWaitOn(std::vector<TaskID> ids) {
  for (auto id : ids) {
    addToWaitOn(id);
  }
}

void RegisteredTask::addToNotify(TaskID id) {
  std::unique_lock<std::shared_mutex> Lock(taskMutex);
  for (auto notifyID : toNotify) {
    if (notifyID == id) return;
  }
  toNotify.push_back(id);
}

void RegisteredTask::addToNotify(std::vector<TaskID> ids) {
  for (auto id : ids) {
    addToNotify(id);
  }
}

void RegisteredTask::setTaskHasRun(TaskID hasRun) {
  std::shared_lock<std::shared_mutex> Lock(taskMutex);
  for (int i = 0; i < waitOn.size(); i++) {
    if (waitOn[i] == hasRun) {
      hasWaitedOn[i] = true;
    }
  }
}

bool RegisteredTask::executeWithID(TaskID id) {
  std::shared_lock<std::shared_mutex> Lock(taskMutex);
  if (!taskPtr) {
    Debug("[RegisteredTask] Tried to execute empty task " + std::to_string(id) +
          ".");
    return false;
  }
  if (!taskPtr || !taskPtr->run(id)) {
    Warn("[RegisteredTask] Execution of task " + std::to_string(id) +
         " failed.");
    return false;
  }
  return true;
}

void RegisteredTask::clearWaitingOn() {
  std::unique_lock<std::shared_mutex> Lock(taskMutex);
  waitOn.clear();
  hasWaitedOn.clear();
}

void RegisteredTask::removeToNotify(TaskID id) {
  std::unique_lock<std::shared_mutex> Lock(taskMutex);
  if (toNotify.size() == 0) return;
  bool needsRedo = false;
  std::vector<TaskID> newVector = std::vector<TaskID>();
  newVector.reserve(toNotify.size() - 1);
  for (auto oldId : toNotify) {
    if (id != oldId)
      newVector.push_back(oldId);
    else
      needsRedo = true;
  }
  if (needsRedo) {
    toNotify = newVector;
  }
}

const std::vector<TaskID> RegisteredTask::getWaitingOn() {
  std::shared_lock<std::shared_mutex> Lock(taskMutex);
  return waitOn;
}

const std::vector<bool> RegisteredTask::getHasWaitedOn() {
  std::shared_lock<std::shared_mutex> Lock(taskMutex);
  return hasWaitedOn;
}

const std::vector<TaskID> RegisteredTask::getToNotify() {
  std::shared_lock<std::shared_mutex> Lock(taskMutex);
  return toNotify;
}


