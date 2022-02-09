#include "TaskSingleton.h"
#include <CeresLog/Logging.h>

std::unique_ptr<Task> TaskSingleton::replaceTask(
    TaskID id, std::unique_ptr<Task> newTask) {
  auto state_lock = std::shared_lock<std::shared_mutex>(stateMutex);
  auto lock = std::unique_lock<std::mutex>(state.tasksMutex);
  if (id >= state.tasks.size()) {
    state.tasks.resize(id);
  }
  return state.tasks[id].replaceTask(std::move(newTask));
}

TaskID TaskSingleton::addTask(std::unique_ptr<Task> task) {
  auto state_lock = std::shared_lock<std::shared_mutex>(stateMutex);
  auto lock = std::unique_lock<std::mutex>(state.tasksMutex);
  auto newID = state.tasks.size();
  state.tasks.emplace_back(std::move(task));
  return newID;
}

std::unique_ptr<Task> TaskSingleton::removeTask(TaskID id) {
  auto state_lock = std::shared_lock<std::shared_mutex>(stateMutex);
  auto lock = std::unique_lock<std::mutex>(state.tasksMutex);
  if (id >= state.tasks.size())
      return nullptr;
  return state.tasks[id].replaceTask(nullptr);
}

TaskState TaskSingleton::replaceState(TaskState newState) {
  auto state_lock = std::unique_lock<std::shared_mutex>(stateMutex);
  TaskState out;
  out = std::move(state);
  state = std::move(newState);
  return out;
}

void TaskSingleton::notifyTaskHasRun(TaskID hasRun, TaskID toNotify) {
  Debug("[TaskSingleton] UNIMPLEMENTED!");
}

void TaskSingleton::setRunnable(TaskID id, bool runnable) {
  Debug("[TaskSingleton] UNIMPLEMENTED!");

}

bool TaskSingleton::setWaitingOn(TaskID waitingTask, TaskID toWaitOn) {
  Debug("[TaskSingleton] UNIMPLEMENTED!");
  return false;
}

bool TaskSingleton::checkHasRun(TaskID id) {
  Debug("[TaskSingleton] UNIMPLEMENTED!");
  return false;
}

bool TaskSingleton::checkIsReady(TaskID id) {
  Debug("[TaskSingleton] UNIMPLEMENTED!");
  return false;
}

bool TaskSingleton::checkIsWaiting(TaskID id) {
  Debug("[TaskSingleton] UNIMPLEMENTED!");
  return false;
}

bool TaskSingleton::waitOnTaskCompletion(TaskID id, size_t timeout) {
  Debug("[TaskSingleton] UNIMPLEMENTED!");
  return false;
}