# Security — Corex

C++20 in-process engine (library). No network surface by itself.

| Topic | Practice |
|-------|----------|
| Memory | Bounded queue capacity; backpressure rejects when full |
| Threads | Join on shutdown; no detached workers owning shared state without sync |
| Input | Callers must not feed untrusted unbounded payloads without their own limits |
| Supply chain | Pin CI toolchain; review FetchContent/GTest updates |

Not a networked service — pair with StreamForge/Pulse for I/O trust boundaries.
