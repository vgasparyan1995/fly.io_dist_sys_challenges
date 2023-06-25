#include "thread_pool.h"

#include <chrono>
#include <thread>
#include <unordered_set>

#include "gtest/gtest.h"

class ThreadPoolTest : public ::testing::Test {
 public:
  auto NewTask(int task_id) {
    return [this, task_id] {
      started_.insert(task_id);
      std::this_thread::sleep_for(std::chrono::seconds(5));
      finished_.insert(task_id);
    };
  }

  std::unordered_set<int> started_;
  std::unordered_set<int> finished_;
};

TEST_F(ThreadPoolTest, SingleThreaded) {
  ThreadPool thread_pool(1);
  thread_pool.AddTask(NewTask(0));
  thread_pool.AddTask(NewTask(1));
  thread_pool.AddTask(NewTask(2));
  std::this_thread::sleep_for(std::chrono::seconds(3));
  EXPECT_TRUE(started_.contains(0));
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(finished_.contains(0));
  EXPECT_TRUE(started_.contains(1));
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(finished_.contains(1));
  EXPECT_TRUE(started_.contains(2));
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(finished_.contains(2));
}

TEST_F(ThreadPoolTest, DefaultNumThreads) {
  const auto kExpectedNumThreads =
      std::max(std::thread::hardware_concurrency(), 1u) * 2;
  ASSERT_GE(kExpectedNumThreads, 2);
  ThreadPool thread_pool;
  thread_pool.AddTask(NewTask(0));
  thread_pool.AddTask(NewTask(1));
  std::this_thread::sleep_for(std::chrono::seconds(2));
  EXPECT_EQ(started_.size(), 2);
  EXPECT_EQ(finished_.size(), 0);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_EQ(started_.size(), 2);
  EXPECT_EQ(finished_.size(), 2);
}
