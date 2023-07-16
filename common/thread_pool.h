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
  ~ThreadPool();

  void AddTask(Task&& task);

 private:
  std::optional<Task> NextTask();

  std::vector<std::jthread> workers_;
  std::mutex mu_tasks_;
  std::queue<Task> tasks_;
  std::condition_variable cond_var_;
};
