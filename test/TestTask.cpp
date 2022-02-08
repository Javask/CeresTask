#include "TestTask.h"

TestTask::TestTask(std::string name) : name(name) {}

void TestTask::run() {

}

const std::string& TestTask::getName() const { return name; }
