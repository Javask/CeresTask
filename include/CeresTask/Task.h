#pragma once

/// <summary>
/// Virtual class that should be inherited from to create an executable task
/// </summary>
class Task {
 protected:
  Task() = default;

 public:
  virtual ~Task() = default;

  virtual void run() = 0;

};