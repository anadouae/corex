#pragma once

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>

namespace corex {

/// Thread-safe bounded MPMC queue with blocking push/pop and try_* variants.
template <typename T>
class BoundedQueue {
public:
  explicit BoundedQueue(std::size_t capacity) : capacity_(capacity) {}

  bool try_push(T value) {
    std::lock_guard lock(mu_);
    if (closed_ || queue_.size() >= capacity_) {
      return false;
    }
    queue_.push(std::move(value));
    cv_not_empty_.notify_one();
    return true;
  }

  /// Blocks until space is available or the queue is closed.
  /// Returns false if closed before the value could be pushed.
  bool push(T value) {
    std::unique_lock lock(mu_);
    cv_not_full_.wait(lock, [&] { return closed_ || queue_.size() < capacity_; });
    if (closed_) {
      return false;
    }
    queue_.push(std::move(value));
    cv_not_empty_.notify_one();
    return true;
  }

  std::optional<T> try_pop() {
    std::lock_guard lock(mu_);
    if (queue_.empty()) {
      return std::nullopt;
    }
    T value = std::move(queue_.front());
    queue_.pop();
    cv_not_full_.notify_one();
    return value;
  }

  /// Blocks until an item is available or the queue is closed and empty.
  std::optional<T> pop() {
    std::unique_lock lock(mu_);
    cv_not_empty_.wait(lock, [&] { return closed_ || !queue_.empty(); });
    if (queue_.empty()) {
      return std::nullopt;
    }
    T value = std::move(queue_.front());
    queue_.pop();
    cv_not_full_.notify_one();
    return value;
  }

  void close() {
    {
      std::lock_guard lock(mu_);
      closed_ = true;
    }
    cv_not_empty_.notify_all();
    cv_not_full_.notify_all();
  }

  bool closed() const {
    std::lock_guard lock(mu_);
    return closed_;
  }

  std::size_t size() const {
    std::lock_guard lock(mu_);
    return queue_.size();
  }

  std::size_t capacity() const { return capacity_; }

private:
  const std::size_t capacity_;
  mutable std::mutex mu_;
  std::condition_variable cv_not_empty_;
  std::condition_variable cv_not_full_;
  std::queue<T> queue_;
  bool closed_ = false;
};

}  // namespace corex
