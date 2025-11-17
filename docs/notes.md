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

## Session Notes

### November 14, 2024 - Phase 1.1-1.3 Complete

**Completed:**
- ✅ Project structure with CMake
- ✅ GoogleTest integration with FetchContent
- ✅ Transition template struct (first real code!)
- ✅ clang-format setup and usage

**Key Learnings:**
1. **Templates must be header-only** - Compiler needs full definition to instantiate
2. **typename vs class in templates** - Identical for parameters, just style
3. **struct vs class** - Semantic signal: struct = data, class = behavior
4. **Template instantiation cost** - Each type combo generates new code
5. **Default initialization** - Only sensible defaults (priority=1.0f), not everything

**Design Decisions:**
- Used `struct` for Transition (passive data container)
- Only defaulted `priority` member (other defaults don't make sense)
- Chose `typename` over `class` for clarity
- Followed Google C++ style throughout

**Discussion Highlights:**
- FetchContent vs git submodules (modern vs traditional)
- Test independence prevents cascading failures
- EXPECT vs ASSERT (soft vs hard failures)
- Automatic test discovery saves manual registration

**Next Up:** Thread safety with mutexes (ROB-9)

### November 17, 2024 - Phase 1.4 Complete

**Completed:**
- ✅ Circular buffer template class
- ✅ Wraparound logic with modulo operator
- ✅ Head/tail pointer management
- ✅ Comprehensive testing (15 tests passing)

**Key Learnings:**
1. **RAII in action** - std::vector handles all cleanup automatically
2. **Rule of Zero** - No need for destructor, copy/move constructors when using containers
3. **reserve() vs resize()** - reserve() allocates space, resize() creates elements
4. **Template classes** - More complex than template structs, but same principles
5. **Header-only requirement for templates** - Compiler needs full definition to instantiate
6. **Const correctness matters** - Both const and non-const overloads for accessors

**Bug Fixed:**
- Originally used `reserve()` instead of `resize()` causing undefined behavior
- Fixed by using `resize()` to actually create elements in vector

**Design Decisions:**
- Chose head/tail pointer approach over single write index
- Used `std::vector` for storage (RAII benefits)
- Made all query methods const-correct
- Comprehensive bounds checking with exceptions

**Discussion Highlights:**
- Why templates MUST be in headers (compilation model)
- reserve() vs resize() for std::vector
- Explicit template instantiation (rarely used)
- Chrome's philosophy: "Pay at compile time, not at runtime"

### November 17, 2024 (continued) - Phase 1.5 Complete (ROB-9)

**Completed:**
- ✅ Thread safety with std::shared_mutex
- ✅ Reader-writer lock pattern implementation
- ✅ Thread safety tests (concurrent adds, reads/writes, queries)
- ✅ Fixed deadlock bug by inlining condition checks

**Key Learnings:**
1. **std::shared_mutex** - Reader-writer locks allow multiple concurrent readers
2. **Lock types** - std::lock_guard (exclusive), std::shared_lock (shared)
3. **RAII locking** - Automatic unlock via destructors, exception-safe
4. **mutable keyword** - Allows mutex locking in const methods
5. **Deadlock prevention** - Never call locking methods from within locked sections
6. **std::atomic** - Lock-free coordination for thread communication

**Bug Fixed:**
- Deadlock in `add()` method caused by calling `is_full()` (which acquires lock) while already holding the lock
- Resolved by inlining the check: changed `if (!is_full())` to `if (size_ != capacity_)`
- This avoids recursive locking on non-recursive mutex

**Design Decisions:**
- Used std::shared_mutex instead of std::mutex (better for read-heavy workloads)
- Non-const accessors use exclusive lock (caller might modify)
- Const accessors use shared lock (read-only, allows concurrency)
- Made mutex mutable to allow locking in const methods

**Discussion Highlights:**
- Deadlock diagnosis and prevention strategies
- Reader-writer lock benefits for replay buffer use case
- Trade-off: inlining checks vs private helper methods (chose inline for simplicity)
- Chrome's extensive use of locks (base::Lock, base::AutoLock)

**Next Up:** Uniform random sampling (ROB-10)

## Open Questions
- Should we use std::pmr for memory pools later?
- Lock-free circular buffer possible without boost::lockfree?
- Best way to handle variable-size observations efficiently?

## Resources
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [C++20 Reference](https://en.cppreference.com/)
- [Prioritized Experience Replay Paper](https://arxiv.org/abs/1511.05952)
