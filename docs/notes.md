# Development Notes

## Project Setup Notes

### Modern C++ Best Practices for This Project
1. **Use C++20 features** where appropriate (concepts, ranges, std::span)
2. **RAII everywhere** - no raw pointers, prefer smart pointers
3. **Template-heavy design** for zero-cost abstractions
4. **Modern concurrency** - prefer std::atomic and lock-free when possible
5. **Google C++ Style Guide** - consistent naming and formatting

### Key Learnings for Chrome Preparation

#### CMake Build System
- Chrome uses GN/Ninja, but CMake teaches similar concepts
- Understand target dependencies and compilation flags
- Practice with FetchContent for dependencies (similar to DEPS in Chrome)

#### C++ Templates
- Chrome heavily uses templates for type safety and performance
- Learn template specialization and SFINAE
- Understand when templates increase compile time vs runtime performance

#### Concurrency Patterns
- Chrome is multi-process AND multi-threaded
- Reader-writer locks are common (similar to base::Lock in Chrome)
- Atomic operations for lock-free data structures

#### Memory Management
- Chrome has strict memory management (no leaks!)
- RAII is critical - every resource needs clear ownership
- Smart pointers (std::unique_ptr, std::shared_ptr) everywhere

### IPC Considerations
- Start with threads (easier debugging)
- Move to shared memory (more Chrome-like)
- Eventually sockets for distributed systems

### Performance Focus
- Always benchmark before optimizing
- Cache-friendly data structures matter
- M1 has unique performance characteristics (ARM64)

## Development Environment

### Compiler Setup
- Using Clang on macOS (same as Chrome)
- C++20 support: `clang++ -std=c++20`
- Warning flags: `-Wall -Wextra -Wpedantic`

### Tools Installed
- clang-format (Google style)
- clang-tidy (static analysis)
- GoogleTest (unit testing)
- Google Benchmark (performance)

## Phase History

### Phase 1: Core Single-Process Implementation ✅ COMPLETED
**Duration:** November 14-19, 2024 (5 days)

**What We Built:**
- Circular buffer with automatic wraparound (ROB-8)
- Thread safety with std::shared_mutex reader-writer locks (ROB-9)
- Uniform random sampling with modern C++ `<random>` (ROB-10)
- Comprehensive benchmarking suite (ROB-11)

**Key Technical Achievements:**
- 26 unit tests (100% pass rate)
- Performance: 19ns add, 300-516ns sample, 52M ops/sec throughput
- All targets exceeded by 2-50x
- Thread-safe for concurrent access
- O(1) operations confirmed

**Major Learnings:**
- Template-based design (header-only libraries)
- std::shared_mutex for read-heavy workloads
- Modern RNG (std::mt19937, std::uniform_int_distribution)
- Deadlock prevention (don't call locking methods from locked contexts)
- Google Benchmark for microbenchmarking
- CMake FetchContent for dependencies

**Critical Bugs Fixed:**
- reserve() vs resize() for std::vector initialization
- Deadlock from calling is_full() within locked add() method
- Solved by inlining condition checks

**Instruction File Feedback:**
- Future guides should explain concepts at high level, not write solution code
- Provide hints about functions/patterns, but let user figure out implementation
- Focus on learning objectives, not code-to-copy

### Phase 2: Prioritized Replay (In Progress)
**Started:** November 20, 2024

**Completed Work:**
- ✅ ROB-12: Sum-tree data structure (November 26, 2024)

**In Progress:**
- ROB-13: Prioritized replay buffer

**Planned Work:**
- ROB-14: Priority updates
- ROB-15: Importance sampling weights
- ROB-16: Benchmarking prioritized sampling

#### ROB-12: Sum-Tree Data Structure ✅
**Completed:** November 26, 2024

**Implementation:**
- Created `include/replay_buffer/sum_tree.h` - fixed-capacity binary tree for O(log n) proportional sampling
- Array-based storage using 0-indexed convention (root at index 0, leaves start at capacity-1)
- Core operations: `set()`, `get()`, `sample()`, `total()` - all O(log n) or better
- Prefix-sum sampling algorithm for proportional selection
- Delta propagation for efficient priority updates

**Key Design Decisions:**
- Not templated - stores `float` priorities only (SumTree is orthogonal to data storage)
- No thread safety - deferred to wrapper layer for coordinated locking
- No size tracking - fixed capacity, wrapper handles size logic
- Bounds checking on capacity only

**Bugs Fixed:**
- Infinite loop from unsigned arithmetic in parent traversal (`while (parent >= 0)` wrapped)
- Wrong node comparison in `sample()` (compared current node instead of left child)
- Removed `size_` member to avoid synchronization complexity

**Tests Created:**
- `tests/sum_tree_test.cpp` with basic functionality tests
- Construction, set/get, total, sampling, tree propagation verified
- User opted to skip statistical distribution tests (confident in implementation)

**Learnings:**
- Binary tree index arithmetic (parent/child calculations)
- Prefix-sum search algorithm
- Array-based tree storage (cache-friendly, no pointer overhead)
- Separation of concerns (priorities vs data storage)

## Open Questions
- Should we use std::pmr for memory pools later?
- Lock-free circular buffer possible without boost::lockfree?
- Best way to handle variable-size observations efficiently?

## Resources
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [C++20 Reference](https://en.cppreference.com/)
- [Prioritized Experience Replay Paper](https://arxiv.org/abs/1511.05952)
