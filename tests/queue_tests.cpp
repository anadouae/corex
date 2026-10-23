#include "corex/bounded_queue.hpp"

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

TEST(BoundedQueue, PushPop) {
  corex::BoundedQueue<int> q(2);
  EXPECT_TRUE(q.try_push(1));
  EXPECT_TRUE(q.try_push(2));
  EXPECT_FALSE(q.try_push(3));
  EXPECT_EQ(q.try_pop().value(), 1);
  EXPECT_EQ(q.try_pop().value(), 2);
  EXPECT_FALSE(q.try_pop().has_value());
}

TEST(BoundedQueue, CloseUnblocksPop) {
  corex::BoundedQueue<int> q(1);
  std::thread t([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    q.close();
  });
  auto v = q.pop();
  EXPECT_FALSE(v.has_value());
  t.join();
}

TEST(BoundedQueue, ConcurrentProducersConsumers) {
  corex::BoundedQueue<int> q(64);
  constexpr int N = 1000;
  std::atomic<int> sum{0};
  std::thread producers[4];
  std::thread consumers[4];
  for (int p = 0; p < 4; ++p) {
    producers[p] = std::thread([&, p] {
      for (int i = p; i < N; i += 4) {
        while (!q.push(i)) {
        }
      }
    });
  }
  for (int c = 0; c < 4; ++c) {
    consumers[c] = std::thread([&] {
      for (;;) {
        auto v = q.pop();
        if (!v) break;
        sum.fetch_add(*v, std::memory_order_relaxed);
      }
    });
  }
  for (auto& t : producers) t.join();
  q.close();
  for (auto& t : consumers) t.join();
  EXPECT_EQ(sum.load(), (N - 1) * N / 2);
}
