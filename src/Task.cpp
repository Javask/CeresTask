#include "../include/CeresTask/Task.h"
#include <Utilities/StringUtils.h>

namespace CeresTask {
Task::Task() : Task(Utilities::generateRandomString(12)) {}

Task::Task(std::string name) : name(std::move(name)) {}

const std::string& Task::getName() const { return name; }

void Task::onFail() {}

const TaskID& Task::getID() const { return id; }

}  // namespace CeresTask