#include "corex/engine.hpp"

namespace corex {

Engine::Engine(std::size_t workers, std::size_t queue_capacity)
    : pool_(workers, queue_capacity) {}

bool Engine::accept(Event event) {
  return pool_.submit([this, event = std::move(event)]() mutable {
    // Deterministic transform: scale value (demo processing cost).
    double out = event.value * 1.0;
    for (int i = 0; i < 32; ++i) {
      out = out * 1.000001 + 0.000001;
    }
    value_sum_x100_.fetch_add(static_cast<std::uint64_t>(out * 100.0), std::memory_order_relaxed);
  });
}

void Engine::shutdown() { pool_.shutdown(); }

EngineStats Engine::stats() const {
  return EngineStats{
      pool_.submitted(),
      pool_.completed(),
      pool_.rejected(),
      value_sum_x100_.load(),
      pool_.depth(),
  };
}

}  // namespace corex
