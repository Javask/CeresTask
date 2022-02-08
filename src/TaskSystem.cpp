#include "../include/CeresTask/TaskSystem.h"
#include "TaskSingleton.h"

TaskID TaskSystem::addTask(std::unique_ptr<Task> task) {
  return TaskSingleton::Get()->addTask(std::move(task));
}

std::unique_ptr<Task> TaskSystem::replaceTask(TaskID id,
                                              std::unique_ptr<Task> newTask) {
  return TaskSingleton::Get()->replaceTask(id, std::move(newTask));
}

std::unique_ptr<Task> TaskSystem::removeTask(TaskID id) {
  return TaskSingleton::Get()->removeTask(std::move(id));
}
