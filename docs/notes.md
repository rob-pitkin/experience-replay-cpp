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

**Next Up:** Circular buffer implementation (ROB-8)

## Open Questions
- Should we use std::pmr for memory pools later?
- Lock-free circular buffer possible without boost::lockfree?
- Best way to handle variable-size observations efficiently?

## Resources
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [C++20 Reference](https://en.cppreference.com/)
- [Prioritized Experience Replay Paper](https://arxiv.org/abs/1511.05952)
