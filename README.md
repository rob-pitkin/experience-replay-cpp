# Experience Replay Buffer in Modern C++

A high-performance, multi-process experience replay buffer service for reinforcement learning applications.

## Project Status

**Current Phase:** Phase 3 - Multi-Process IPC
**Current Task:** [ROB-17](https://linear.app/robpersonal/issue/ROB-17) - Shared Memory Foundation
**Started:** November 29, 2024

### Completed
- ✅ Phase 1: Core circular buffer with uniform sampling (ROB-5 through ROB-11)
- ✅ Phase 2: Prioritized replay buffer (ROB-12, ROB-13, ROB-16)
  - Sum-tree data structure with O(log n) operations
  - Proportional prioritized sampling with importance sampling weights
  - Benchmarks confirm < 10μs latency for batch of 32

### In Progress
- 🔄 Phase 3: Multi-process IPC implementation (ROB-17 through ROB-21)
  - POSIX shared memory for cross-process buffer access
  - Process-shared synchronization primitives
  - SharedCircularBuffer and SharedPrioritizedReplayBuffer

### Future Enhancements
- Beta annealing schedule (client-side responsibility)
- Phase 4: Python bindings with pybind11
- Phase 5: Advanced features (n-step returns, compression)

## Learning Goals

- Master modern C++ (C++20/23)
- Understand IPC mechanisms (shared memory, sockets)
- Practice RL infrastructure development
- Build production-quality concurrent data structures

## Getting Started

1. **Read the setup instructions:** [.claude/SETUP_INSTRUCTIONS.md](.claude/SETUP_INSTRUCTIONS.md)
2. **Follow the TODOs** to create your CMake configuration
3. **Ask Claude** any questions about C++, CMake, or design decisions

Note: Task-specific instruction files are in the `.claude/` directory (gitignored)

## Documentation

- [Project Specification](docs/replay_buffer_spec.md) - Full technical requirements
- [Timeline](docs/timeline.md) - Current progress and roadmap
- [CMake Primer](docs/cmake_primer.md) - Quick reference for build system
- [POSIX IPC Primer](docs/posix_ipc_primer.md) - Shared memory and synchronization guide
- [Development Notes](docs/notes.md) - Learning notes and design decisions

## Build & Test

```bash
# Configure and build
mkdir build && cd build
cmake ..
cmake --build .

# Run tests
ctest --output-on-failure

# Run benchmarks
./benchmarks/replay_buffer_benchmarks

# Run specific benchmarks (filter by name)
./benchmarks/replay_buffer_benchmarks --benchmark_filter=Prioritized

# Run with repetitions for statistical confidence
./benchmarks/replay_buffer_benchmarks --benchmark_repetitions=3

# Save benchmark results to file
./benchmarks/replay_buffer_benchmarks --benchmark_out=results.json --benchmark_out_format=json
```

## Tools Required

- CMake 3.20+
- Clang with C++20 support (comes with Xcode on macOS)
- clang-format and clang-tidy (for code style)

## Project Structure

```
include/replay_buffer/  # Public API headers
src/                    # Implementation files
tests/                  # Unit tests
benchmarks/             # Performance benchmarks
docs/                   # Documentation
```

