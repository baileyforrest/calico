#pragma once

#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#include "src/base/macros.h"

// A basic TaskRunner to run tasks asynchronously in order.
class TaskRunner {
 public:
  using Closure = std::function<void()>;
  using Duration = std::chrono::duration<int64_t>;

  TaskRunner();

  // The destructor blocks until the current task running is complete. All other
  // tasks are dropped.
  ~TaskRunner();

  void PostTask(Closure f);
  void PostDelayedTask(Closure f, const Duration& delay);
  void WaitUntilIdle();

 private:
  using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

  void RunLoop();

  // The thread tasks run on.
  std::thread thread_;

  // Mutex for following members:
  std::mutex mutex_;

  // Used to notify to RunLoop a new task is available.
  std::condition_variable has_task_cv_;

  // Used to notify threads blocked on WaitUntilIdle.
  std::condition_variable is_idle_cv_;

  // True if the task runner is still running.
  bool running_ = true;

  std::queue<Closure> tasks_;
  std::deque<std::pair<SteadyTimePoint, Closure>> delayed_tasks_;

  DISALLOW_MOVE_COPY_AND_ASSIGN(TaskRunner);
};
