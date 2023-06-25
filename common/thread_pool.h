#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  using Task = std::function<void()>;

  explicit ThreadPool();
  explicit ThreadPool(int num_threads);

  void AddTask(Task&& task);

 private:
  std::vector<std::jthread> workers_;
  std::mutex mu_tasks_;
  std::condition_variable task_ready_;
  std::queue<Task> tasks_;
};
