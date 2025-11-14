# Experience Replay Buffer in Modern C++

A high-performance, multi-process experience replay buffer service for reinforcement learning applications.

## Project Status

**Current Phase:** Phase 1 - Core Single-Process Implementation
**Current Task:** [ROB-5](https://linear.app/robpersonal/issue/ROB-5) - Project Setup

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
- [Development Notes](docs/notes.md) - Learning notes and design decisions

## Build

```bash
# Configure
mkdir build && cd build
cmake ..

# Build
cmake --build .

# Run tests (after Phase 1.2)
ctest --output-on-failure
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

