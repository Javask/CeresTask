#pragma once
#include <cstdint>
#include <atomic>
#include <string>

namespace CeresTask {

/// <summary>
/// Create a global TaskID type
/// </summary>
class TaskID {
 public:
  /// <summary>
  /// Create a runtime unique TaskID
  /// </summary>
  TaskID();

  TaskID(const TaskID& other) = default;
  TaskID(TaskID&& other) = default;

  TaskID& operator=(const TaskID& other) noexcept = default;
  TaskID& operator=(TaskID&& other) noexcept = default;

  ~TaskID() = default;

  ///< summary>
  /// Comparison operator comparing numerical value
  ///</summary>
  ///< returns>Three way comparison result of id comparison</returns>
  auto operator<=>(const TaskID& other) const = default;

  ///< summary>
  /// See toString()
  ///</summary>
  ///< returns> Numerical value as string </returns>
  std::string operator()() const;

  ///< summary>
  /// Converts id to string
  ///</summary>
  ///< returns> Numerical value as string </returns>
  [[nodiscard]] std::string toString() const;

 private:
  intmax_t id;

  static std::atomic<intmax_t> TaskIDCounter;
  static intmax_t getNewID();
};

}  // namespace CeresTask
