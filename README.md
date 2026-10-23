# COREX — High-Performance Event Processing Engine

Modern **C++20** event engine: bounded queue, worker pool, backpressure, graceful shutdown, tests, and reproducible benchmarks.

Part of Douae El Assal’s portfolio ecosystem (feeds Urban Pulse / StreamForge later).

## Build

```bash
cmake -S . -B build -DCOREX_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/corex_bench
```

## What it proves

- Thread-safe bounded MPMC queue
- Worker pool with submit/reject metrics
- Backpressure when the queue is full
- GoogleTest coverage + sanitizer-friendly code
- Measured throughput via `corex_bench`

## Author

Douae El Assal · ENSA Tétouan · https://github.com/anadouae
