# cpp-cache

A thread-safe LFU (Least Frequently Used) cache implementation in modern C++.

## Features

- O(1) average time complexity for `get`, `put`, and eviction
- Frequency-bucket design using doubly-linked lists
- Deterministic eviction policy
- RAII-based memory management (no raw ownership)
- Sharded version (`KHashLfuCache`) to reduce lock contention
- Catch2 test coverage

## Architecture

**LfuCache**
- `unordered_map<Key, Node>` for direct lookup
- `unordered_map<int, FreqList>` for frequency buckets
- `std::mutex` for thread safety

**KHashLfuCache**
- Key sharding via `std::hash`
- Independent LFU slices per shard

## Build

Requires C++17 and CMake.

```bash
cmake -S . -B build
cmake --build build
ctest