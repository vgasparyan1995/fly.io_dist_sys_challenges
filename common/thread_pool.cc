#include "thread_pool.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <mutex>
#include <stop_token>
#include <thread>

namespace {

int NumThreads() {
  return std::max(std::thread::hardware_concurrency(), 1u) * 2;
}

}  // namespace

ThreadPool::ThreadPool() : ThreadPool(NumThreads()) {}

ThreadPool::ThreadPool(int num_threads) {
  for (int i = 0; i < num_threads; ++i) {
    workers_.emplace_back([this](std::stop_token stoken) {
      while (!stoken.stop_requested()) {
        std::optional<Task> task;
        {
          std::unique_lock lck{mu_tasks_};
          if (tasks_.empty()) {
            cond_var_.wait(lck);
            if (stoken.stop_requested()) {
              return;
            }
          }
          task = NextTask();
        }
        if (task) {
          (*task)();
        }
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  std::unique_lock lck{mu_tasks_};
  for (auto& worker : workers_) {
    worker.request_stop();
  }
  cond_var_.notify_all();
}

void ThreadPool::AddTask(Task&& task) {
  std::unique_lock lck{mu_tasks_};
  tasks_.push(std::move(task));
  cond_var_.notify_one();
}

std::optional<ThreadPool::Task> ThreadPool::NextTask() {
  assert(!mu_tasks_.try_lock());
  std::optional<Task> next_task;
  if (tasks_.empty()) {
    return next_task;
  }
  next_task = std::move(tasks_.front());
  tasks_.pop();
  return next_task;
}
