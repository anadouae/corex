#pragma once

#include "corex/bounded_queue.hpp"

#include <atomic>
#include <functional>
#include <thread>
#include <vector>

namespace corex {

class WorkerPool {
public:
  using Task = std::function<void()>;

  WorkerPool(std::size_t workers, std::size_t queue_capacity)
      : queue_(queue_capacity) {
    workers_.reserve(workers);
    for (std::size_t i = 0; i < workers; ++i) {
      workers_.emplace_back([this] { run(); });
    }
  }

  ~WorkerPool() { shutdown(); }

  WorkerPool(const WorkerPool&) = delete;
  WorkerPool& operator=(const WorkerPool&) = delete;

  bool submit(Task task) {
    submitted_.fetch_add(1, std::memory_order_relaxed);
    if (!queue_.push(std::move(task))) {
      rejected_.fetch_add(1, std::memory_order_relaxed);
      return false;
    }
    return true;
  }

  void shutdown() {
    bool expected = false;
    if (!shutting_down_.compare_exchange_strong(expected, true)) {
      return;
    }
    queue_.close();
    for (auto& t : workers_) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

  std::uint64_t submitted() const { return submitted_.load(); }
  std::uint64_t completed() const { return completed_.load(); }
  std::uint64_t rejected() const { return rejected_.load(); }
  std::size_t depth() const { return queue_.size(); }

private:
  void run() {
    while (true) {
      auto task = queue_.pop();
      if (!task) {
        break;
      }
      (*task)();
      completed_.fetch_add(1, std::memory_order_relaxed);
    }
  }

  BoundedQueue<Task> queue_;
  std::vector<std::thread> workers_;
  std::atomic<bool> shutting_down_{false};
  std::atomic<std::uint64_t> submitted_{0};
  std::atomic<std::uint64_t> completed_{0};
  std::atomic<std::uint64_t> rejected_{0};
};

}  // namespace corex
