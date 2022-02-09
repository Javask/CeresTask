#include "../include/CeresTask/Task.h"
#include "RandomString.h"

Task::Task(bool runOnce) : Task(generateRandomString(12), runOnce) {}

Task::Task(const std::string& name, bool runOnce)
    : name(name), runOnce(runOnce) {}

const std::string& Task::getName() const { return name; }