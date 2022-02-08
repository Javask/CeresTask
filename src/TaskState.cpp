#include "TaskState.h"

TaskState &TaskState::operator=(TaskState && other) { 
	auto lock = std::unique_lock<std::mutex>(other.tasksMutex);
	this->tasks =std::move(other.tasks);
	return *this; 
}

TaskState::TaskState(TaskState &&other) {
  auto lock = std::unique_lock<std::mutex>(other.tasksMutex);
  this->tasks = std::move(other.tasks);
}