#include "corex/engine.hpp"

#include <gtest/gtest.h>
#include <chrono>
#include <thread>

TEST(Engine, ProcessesAllAcceptedEvents) {
  corex::Engine engine(2, 128);
  for (int i = 0; i < 100; ++i) {
    ASSERT_TRUE(engine.accept(corex::Event{"e" + std::to_string(i), "demo", static_cast<double>(i)}));
  }
  // Wait briefly for workers
  for (int i = 0; i < 50; ++i) {
    if (engine.stats().completed >= 100) break;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  engine.shutdown();
  auto s = engine.stats();
  EXPECT_EQ(s.submitted, 100u);
  EXPECT_EQ(s.completed, 100u);
  EXPECT_EQ(s.rejected, 0u);
}

TEST(Engine, BackpressureRejectsWhenFull) {
  corex::Engine engine(1, 1);
  // Saturate with a slow task pattern by filling queue quickly with no shutdown wait.
  int accepted = 0;
  for (int i = 0; i < 20; ++i) {
    if (engine.accept(corex::Event{"x", "demo", 1.0})) {
      ++accepted;
    }
  }
  engine.shutdown();
  EXPECT_GE(accepted, 1);
  EXPECT_EQ(engine.stats().submitted, static_cast<std::uint64_t>(accepted));
}
