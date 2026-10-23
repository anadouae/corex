#pragma once

#include "corex/worker_pool.hpp"

#include <cstdint>
#include <string>

namespace corex {

struct Event {
  std::string id;
  std::string type;
  double value = 0.0;
};

struct EngineStats {
  std::uint64_t submitted = 0;
  std::uint64_t completed = 0;
  std::uint64_t rejected = 0;
  std::uint64_t processed_value_sum_x100 = 0;
  std::size_t queue_depth = 0;
};

/// Minimal high-performance event processing engine used as COREX MVP.
class Engine {
public:
  Engine(std::size_t workers, std::size_t queue_capacity);

  bool accept(Event event);
  void shutdown();
  EngineStats stats() const;

private:
  WorkerPool pool_;
  std::atomic<std::uint64_t> value_sum_x100_{0};
};

}  // namespace corex
