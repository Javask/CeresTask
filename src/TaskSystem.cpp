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
  return TaskSingleton::Get()->removeTask(id);
}

void TaskSystem::setRunnable(TaskID id, bool runnable) {
  return TaskSingleton::Get()->setRunnable(id,runnable);
}

bool TaskSystem::setWaitingOn(TaskID waitingTask, TaskID toWaitOn) {
  return TaskSingleton::Get()->setWaitingOn(waitingTask,toWaitOn);
}

bool TaskSystem::checkHasRun(TaskID id) {
  return TaskSingleton::Get()->checkHasRun(id);
}

bool TaskSystem::checkIsReady(TaskID id) {
  return TaskSingleton::Get()->checkIsReady(id);
}

bool TaskSystem::checkIsWaiting(TaskID id) {
  return TaskSingleton::Get()->checkIsWaiting(id);
}

bool TaskSystem::waitOnTaskCompletion(TaskID id, size_t timeout) {
  return TaskSingleton::Get()->waitOnTaskCompletion(id, timeout);
}