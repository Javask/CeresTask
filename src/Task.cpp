#include "../include/CeresTask/Task.h"
#include "RandomString.h"

Task::Task(bool runOnce) : Task(generateRandomString(12), runOnce) {}

Task::Task(std::string name, bool runOnce)
    : name(std::move(name)), runOnce(runOnce) {}

const std::string& Task::getName() const { return name; }

void Task::onFail() {}

const TaskID& Task::getID() const { return id; }