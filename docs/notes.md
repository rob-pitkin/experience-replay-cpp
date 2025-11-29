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

### Phase 2: Prioritized Replay ✅ COMPLETED
**Duration:** November 20-29, 2024 (9 days)

**Completed Work:**
- ✅ ROB-12: Sum-tree data structure (November 26, 2024)
- ✅ ROB-13: Prioritized replay buffer (November 29, 2024)
- ✅ ROB-16: Benchmarking prioritized sampling (November 29, 2024)

**Note:** ROB-14 (priority updates) and ROB-15 (importance sampling weights) were covered by ROB-13 implementation

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

#### ROB-13: Prioritized Replay Buffer ✅
**Completed:** November 29, 2024

**Implementation:**
- Created `include/replay_buffer/prioritized_replay_buffer.h` - integrates CircularBuffer and SumTree for proportional prioritized sampling
- Wrapper architecture: coordinates two data structures with single std::shared_mutex
- Core operations: `add()`, `sample()`, `update_priorities()` with importance sampling weights
- Configuration: alpha (priority exponent), beta (IS weight exponent), epsilon (stability constant)
- Returns `PrioritizedSample` struct with transition, weight, and index

**Key Design Decisions:**
- Modified CircularBuffer::add() to return physical index (needed for sum-tree coordination)
- Wrapper-level locking (neither CircularBuffer nor SumTree need own locks)
- Member RNG (`mutable std::mt19937 gen_`) to avoid creating new RNG each call
- New transitions get max_priority to ensure they're sampled at least once
- Priority transformation: `(|TD-error| + ε)^α` applied in update_priorities()
- IS weight formula: `weight = (N × P(i))^(-β)` where N = buffer.size()

**Critical Implementation Details:**
- Constructor initializer list order matches member declaration order
- max_priority default: 1.0f (reasonable middle ground)
- Alpha exponent applied correctly: epsilon added BEFORE alpha exponent
- max_priority comparison with transformed priority, not raw TD-error
- Used exclusive locks for sample() due to non-thread-safe RNG

**Bugs Fixed:**
- Constructor member initialization order (had to use initializer list, not assignment)
- IS weight calculation used batch_size instead of buffer.size() (N should be total buffer size)
- Alpha exponent order (epsilon added after exponent instead of before)
- max_priority comparison with raw td_error instead of transformed priority

**Tests Created:**
- `tests/prioritized_replay_buffer_test.cpp` with 5 comprehensive tests
- Construction test with config validation
- Invalid construction test (all error cases)
- Add transitions test (size tracking)
- Sample test (structure validation)
- **Proportional distribution test** (THE CRITICAL TEST):
  - Set priorities [1, 2, 3, 4] with alpha=1.0 (direct proportionality)
  - Sample 10,000 times and verify distribution is ~10%, 20%, 30%, 40% (±5%)
  - This validates the entire priority sampling pipeline!

**Learnings:**
- Data structure composition (combining specialized structures)
- Probability and sampling (discrete distributions, inverse transform sampling)
- Importance sampling for bias correction
- Coordinated thread safety across multiple structures
- When to use exclusive vs shared locks
- Physical vs logical indexing in circular buffers
- Priority transformation pipeline (TD-error → transformed priority → sampling probability → IS weight)
- Statistical testing to validate probabilistic algorithms

**Performance:**
- All 40 tests pass (100% pass rate)
- Implementation complete and correct

#### ROB-16: Benchmarking Prioritized Sampling ✅
**Completed:** November 29, 2024

**Implementation:**
- Created comprehensive benchmarks in `benchmarks/prioritized_replay_buffer_benchmark.cpp`
- 6 total benchmarks: add (single/concurrent), sample (single/concurrent), update_priorities (single/concurrent)
- All benchmarks use batch size = 32 for realistic RL workload simulation
- Tested buffer sizes: 1K, 100K, 1M elements

**Key Results (DEBUG build):**
- **add()**: ~90 ns (5x slower than CircularBuffer's 19ns) ✅
- **sample(32)**: ~8,900 ns (meets < 10μs target!) ✅
- **update_priorities(32)**: ~1,620 ns (excellent performance) ✅

**Complexity Verification:**
- sample(32) time nearly constant across buffer sizes (1K: 8,895ns, 100K: 8,892ns, 1M: 8,864ns)
- Confirms O(log n) behavior - logarithmic growth hidden in measurement noise
- Expected: log₂(1K)=10, log₂(100K)=17, log₂(1M)=20 (only 2x difference)

**Overhead Analysis:**
- Prioritized add: 5x slower than uniform (acceptable - includes tree update)
- Prioritized sample: 15-20x slower than uniform (acceptable - includes tree traversal + IS weights)
- Trade-off worthwhile: get proportional prioritized sampling with importance sampling correction

**Bugs Fixed During Implementation:**
- Setup code running on every benchmark iteration instead of once (caused "Index out of range")
- Static vectors accumulating across runs in sample benchmarks
- Race condition: multiple threads setting `initialized = true` simultaneously
- Race condition: multiple threads filling static vectors in concurrent benchmarks

**Learnings:**
- Google Benchmark framework patterns (State object, iteration control, ThreadRange)
- Importance of static variables and initialization flags in benchmark setup
- Subtle concurrency bugs in multi-threaded benchmarks (when to use thread_index() == 0)
- Empirical verification of algorithmic complexity (O(log n) confirmation)
- Performance profiling and interpretation of benchmark results
- DEBUG vs Release build impact on absolute timing (relative performance unchanged)

### Phase 3: Multi-Process IPC (In Progress)
**Started:** November 29, 2024

**Planned Work:**
- ROB-17: Shared memory foundation (POSIX shm_open, mmap)
- ROB-18: Process synchronization primitives (pthread with PTHREAD_PROCESS_SHARED)
- ROB-19: Shared memory buffer adaptation
- ROB-20: Multi-process connection management
- ROB-21: Multi-process integration tests

**Key Challenges Ahead:**
- Cannot use std::vector or dynamic allocation in shared memory
- Must use process-shared mutexes instead of std::shared_mutex
- Need offset-based addressing (pointers don't work across processes)
- Robust mutex handling for process crash recovery
- Memory layout design for fixed-size structures

**Learning Focus:**
- POSIX shared memory APIs (shm_open, mmap, munmap)
- Process-shared pthread synchronization
- Memory layout and alignment for IPC
- Placement new and explicit object lifetime management
- Multi-process testing patterns

## Open Questions
- Should we use std::pmr for memory pools later?
- Lock-free circular buffer possible without boost::lockfree?
- Best way to handle variable-size observations efficiently?

## Resources
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [C++20 Reference](https://en.cppreference.com/)
- [Prioritized Experience Replay Paper](https://arxiv.org/abs/1511.05952)
