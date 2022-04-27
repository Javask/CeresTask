#include "../include/CeresTask/TaskID.h"

namespace CeresTask {

std::atomic<intmax_t> TaskID::TaskIDCounter = 0;

intmax_t TaskID::getNewID() { return TaskID::TaskIDCounter++; }

TaskID::TaskID() : id(getNewID()) {}

std::string TaskID::operator()() const { return toString(); }

std::string TaskID::toString() const { return std::to_string(this->id); }

}  // namespace CeresTask
