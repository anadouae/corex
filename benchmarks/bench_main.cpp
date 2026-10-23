#include "corex/engine.hpp"

#include <chrono>
#include <iostream>

int main() {
  constexpr int N = 200000;
  corex::Engine engine(4, 4096);
  const auto t0 = std::chrono::steady_clock::now();
  int accepted = 0;
  for (int i = 0; i < N; ++i) {
    if (engine.accept(corex::Event{"e", "bench", 1.0})) {
      ++accepted;
    }
  }
  while (engine.stats().completed < static_cast<std::uint64_t>(accepted)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  engine.shutdown();
  const auto t1 = std::chrono::steady_clock::now();
  const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
  const auto s = engine.stats();
  std::cout << "COREX bench\n"
            << "  accepted=" << accepted << "\n"
            << "  completed=" << s.completed << "\n"
            << "  rejected=" << s.rejected << "\n"
            << "  elapsed_ms=" << ms << "\n"
            << "  throughput_eps=" << (s.completed * 1000.0 / ms) << "\n";
  return 0;
}
