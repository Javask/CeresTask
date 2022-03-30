#include <utility>

#include "../include/CeresTask/TaskGroup.h"
#include "RandomString.h"

TaskGroup::TaskGroup() : TaskGroup("TaskGroup_" + generateRandomString(8)) {}

TaskGroup::TaskGroup(std::string name) : name(std::move(std::move(name))) {}

std::optional<size_t> TaskGroup::getFreeIndex() {
  auto lock = std::unique_lock(freeTaskIndicesMutex);
  if (freeTaskIndices.empty()) return {};

  size_t index = freeTaskIndices.top();
  freeTaskIndices.pop();
  return {index};
}

size_t TaskGroup::addTaskToVector(std::unique_ptr<Task> task) {
  auto lock = std::unique_lock(tasksMutex);
  size_t index = tasks.size();
  tasks.push_back(std::move(task));
  return index;
}

size_t TaskGroup::addTaskToVector(std::unique_ptr<Task> task, size_t index) {
  auto lock = std::unique_lock(tasksMutex);
  tasks[index] = std::move(task);
  return index;
}

void TaskGroup::addTaskToLookup(TaskID id, size_t index) {
  auto lock = std::unique_lock(taskLookupMutex);
  taskLookup[id] = index;
}

void TaskGroup::addTask(std::unique_ptr<Task> task) {
  auto coherent = std::shared_lock(coherencyMutex);
  TaskID id = task->getID();
  auto freeIndex = getFreeIndex();
  size_t index;
  if (!freeIndex.has_value()) {
    index = addTaskToVector(std::move(task));
  } else {
    index = addTaskToVector(std::move(task), *freeIndex);
  }
  addTaskToLookup(id, index);
}

void TaskGroup::addFreeIndex(size_t index) {
  auto lock = std::unique_lock(freeTaskIndicesMutex);
  freeTaskIndices.push(index);
}

std::optional<size_t> TaskGroup::removeTaskFromLookup(TaskID id) {
  auto lock = std::unique_lock(taskLookupMutex);
  if (!taskLookup.contains(id)) return {};
  size_t index = taskLookup.at(id);
  taskLookup.erase(id);
  return {index};
}

std::unique_ptr<Task> TaskGroup::removeTaskFromVector(size_t index) {
  auto lock = std::unique_lock(tasksMutex);
  auto ptr = std::move(tasks[index]);
  tasks[index] = nullptr;
  return ptr;
}

std::unique_ptr<Task> TaskGroup::removeTask(TaskID id) {
  auto coherent = std::shared_lock(coherencyMutex);
  auto index = removeTaskFromLookup(id);
  if (!index) return nullptr;

  auto ptr = removeTaskFromVector(*index);
  addFreeIndex(*index);
  removeTaskFromWaitOn(id);
  return ptr;
}

std::map<TaskID, std::string> TaskGroup::getContainedTaskNames() {
  auto outMap = std::map<TaskID, std::string>();
  auto lock = std::unique_lock(tasksMutex);
  for (auto& ptr : tasks) {
    if (ptr) {
      outMap[ptr->getID()] = ptr->getName();
    }
  }
  return outMap;
}
std::set<TaskID> TaskGroup::getContainedTaskIDs() {
  auto outSet = std::set<TaskID>();
  auto lock = std::unique_lock(tasksMutex);
  for (auto& ptr : tasks) {
    if (ptr) {
      outSet.insert(ptr->getID());
    }
  }
  return outSet;
}

void TaskGroup::clear() {
  auto coherent = std::shared_lock(coherencyMutex);
  auto lock = std::scoped_lock(tasksMutex, freeTaskIndicesMutex,
                               taskLookupMutex, waitOnMutex);
  tasks.clear();
  freeTaskIndices = std::stack<size_t>();
  taskLookup.clear();
  waitOn.clear();
}

void TaskGroup::addToWaitOn(TaskID start, TaskID end) {
  auto lock = std::unique_lock(waitOnMutex);
  for (auto& pair : waitOn) {
    if (pair.first == start && pair.second == end) return;
  }
  waitOn.emplace_back(start, end);
}

void TaskGroup::removeEdgeFromWaitOn(TaskID start, TaskID end) {
  auto lock = std::unique_lock(waitOnMutex);
  for (auto it = waitOn.begin(); it != waitOn.end();) {
    if (it->first == start && it->second == end) {
      it = waitOn.erase(it);
    } else {
      it++;
    }
  }
}

void TaskGroup::removeTaskFromWaitOn(TaskID id) {
  auto lock = std::unique_lock(waitOnMutex);
  for (auto it = waitOn.begin(); it != waitOn.end();) {
    if (it->first == id || it->second == id) {
      it = waitOn.erase(it);
    } else {
      it++;
    }
  }
}

void TaskGroup::removeStartFromWaitOn(TaskID id) {
  auto lock = std::unique_lock(waitOnMutex);
  for (auto it = waitOn.begin(); it != waitOn.end();) {
    if (it->first == id) {
      it = waitOn.erase(it);
    } else {
      it++;
    }
  }
}

void TaskGroup::removeEndFromWaitOn(TaskID id) {
  auto lock = std::unique_lock(waitOnMutex);
  for (auto it = waitOn.begin(); it != waitOn.end();) {
    if (it->second == id) {
      it = waitOn.erase(it);
    } else {
      it++;
    }
  }
}

bool TaskGroup::checkTasksAreInGroup(TaskID start, TaskID end) {
  auto lock = std::unique_lock(taskLookupMutex);
  return (taskLookup.contains(start) && taskLookup.contains(end));
}

TaskGroup::Contents TaskGroup::moveContent() {
  Contents outStruct = {};
  {
    auto coherent = std::unique_lock(coherencyMutex);
    auto lock = std::scoped_lock(taskLookupMutex, tasksMutex,
                                 freeTaskIndicesMutex, waitOnMutex);
    outStruct.taskLookup = std::move(taskLookup);
    outStruct.tasks = std::move(tasks);
    outStruct.waitOn = std::move(waitOn);

    taskLookup = std::map<TaskID, size_t>();
    tasks = std::vector<std::unique_ptr<Task>>();
    waitOn = std::vector<std::pair<TaskID, TaskID>>();
    freeTaskIndices = std::stack<size_t>();
  }
  return outStruct;
}

void TaskGroup::addWaitOnEdge(TaskID waitOnID, TaskID waitingID) {
  auto coherent = std::shared_lock(coherencyMutex);
  if (checkTasksAreInGroup(waitOnID, waitingID)) {
    addToWaitOn(waitOnID, waitingID);
  }
}

const std::string& TaskGroup::getName() { return name; }