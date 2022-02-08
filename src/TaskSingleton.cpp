#include "TaskSingleton.h"

std::unique_ptr<Task> TaskSingleton::replaceTask(
    TaskID id, std::unique_ptr<Task> newTask) {
  auto state_lock = std::shared_lock<std::shared_mutex>(stateMutex);
  auto lock = std::unique_lock<std::mutex>(state.tasksMutex);
  if (id >= state.tasks.size()) {
    state.tasks.resize(id);
  }
  std::unique_ptr<Task> out = std::move(state.tasks[id]);
  state.tasks[id] = std::move(newTask);
  return out;
}

TaskID TaskSingleton::addTask(std::unique_ptr<Task> task) {
  auto state_lock = std::shared_lock<std::shared_mutex>(stateMutex);
  auto lock = std::unique_lock<std::mutex>(state.tasksMutex);
  auto newID = state.tasks.size();
  state.tasks.push_back(std::move(task));
  return newID;
}

std::unique_ptr<Task> TaskSingleton::removeTask(TaskID id) {
  auto state_lock = std::shared_lock<std::shared_mutex>(stateMutex);
  auto lock = std::unique_lock<std::mutex>(state.tasksMutex);
  if (id >= state.tasks.size())
      return nullptr;
  return std::move(state.tasks[id]);
}

TaskState TaskSingleton::replaceState(TaskState newState) {
  auto state_lock = std::unique_lock<std::shared_mutex>(stateMutex);
  TaskState out;
  out = std::move(state);
  state = std::move(newState);
  return std::move(out);
}
