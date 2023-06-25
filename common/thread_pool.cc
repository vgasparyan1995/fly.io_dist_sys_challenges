#include "thread_pool.h"

#include <algorithm>
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
      std::unique_lock lck{mu_tasks_};
      while (task_ready_.wait_for(lck, std::chrono::seconds(1), [this, stoken] {
        return !stoken.stop_requested() && !tasks_.empty();
      })) {
        Task task = std::move(tasks_.front());
        tasks_.pop();
        lck.unlock();
        task();
        lck.lock();
      }
    });
  }
}

void ThreadPool::AddTask(Task&& task) {
  std::unique_lock lck{mu_tasks_};
  tasks_.push(std::move(task));
  if (tasks_.size() == 1) {
    task_ready_.notify_one();
  }
}
