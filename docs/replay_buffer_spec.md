# Experience Replay Buffer Service - Project Specification

## Project Overview

A high-performance, multi-process experience replay buffer service written in modern C++ for reinforcement learning applications. The service provides efficient storage and sampling of experience transitions with support for various sampling strategies and concurrent access patterns.

## Goals

### Primary Goals
- Build a production-quality experience replay buffer in modern C++
- Support multi-process IPC for distributed RL training
- Implement both uniform and prioritized sampling strategies
- Practice modern C++ concurrency patterns and best practices
- Create a reusable component for future RL projects

### Learning Objectives
- Master modern C++ features (C++17/20/23)
- Understand lock-free data structures and concurrent programming
- Gain experience with IPC mechanisms (shared memory, sockets)
- Learn RL-specific data structures (sum trees, circular buffers)
- Practice software architecture and API design

## Technical Requirements

### Hardware Constraints
- Target platform: MacBook Pro M1
- Must be memory-efficient (support buffers up to several GB)
- Optimize for ARM architecture where beneficial

### Software Requirements
- C++20 minimum (C++23 preferred if available)
- CMake for build system
- Unit tests with Google Test or Catch2
- Python bindings (pybind11) for integration with training frameworks
- Cross-platform (macOS primary, Linux secondary)

## Architecture

### Core Components

#### 1. Circular Buffer
- Template-based design for flexible transition types
- Fixed capacity with efficient wraparound
- Zero-copy access where possible using `std::span`
- Support for variable-length observations (images vs. vectors)

#### 2. Sampling Engine
- **Uniform Random Sampling**: O(1) sampling from buffer
- **Prioritized Sampling**: 
  - Sum-tree data structure for O(log n) sampling
  - Support for proportional and rank-based prioritization
  - Configurable alpha (priority exponent) and beta (IS weight) parameters

#### 3. Storage Manager
- Handles memory allocation and layout
- Support for different observation types (float, uint8, etc.)
- Optional compression for image data
- Memory-mapped backing for persistence (optional)

#### 4. IPC Layer
- **Phase 1**: Thread-safe single process (std::shared_mutex, atomics)
- **Phase 2**: Shared memory multi-process (POSIX shm or boost::interprocess)
- **Phase 3**: Socket-based remote access (Unix domain or TCP)
- Lock-free operations where possible

#### 5. Python Bindings
- Clean Python API using pybind11
- NumPy array integration (zero-copy where possible)
- Async/await support for non-blocking operations

### Data Model

```cpp
template<typename Observation, typename Action>
struct Transition {
    Observation observation;
    Action action;
    float reward;
    Observation next_observation;
    bool done;
    float priority;  // For prioritized replay
};
```

## Implementation Phases

### Phase 1: Core Single-Process Implementation (2-3 weeks)
**Deliverables:**
- Circular buffer with uniform sampling
- Basic transition storage (float observations, int actions)
- Thread-safe operations with mutex-based locking
- Unit tests for core functionality
- Simple benchmark suite

**Success Criteria:**
- Can store and sample 1M+ transitions
- Thread-safe for multi-threaded training
- < 1μs latency for sampling operations

### Phase 2: Prioritized Replay (1-2 weeks)
**Deliverables:**
- Sum-tree implementation
- Proportional prioritized sampling
- Priority update mechanism
- Importance sampling weight calculation
- Extended unit tests

**Success Criteria:**
- Correct sampling distribution verification
- O(log n) sampling and update complexity
- Support for dynamic priority updates

### Phase 3: Multi-Process IPC (2-3 weeks)
**Deliverables:**
- Shared memory implementation
- Process synchronization primitives
- Writer/multiple readers support
- IPC configuration and connection management
- Integration tests with multiple processes

**Success Criteria:**
- Multiple processes can read/write concurrently
- Minimal performance overhead vs single-process
- Graceful handling of process crashes

### Phase 4: Python Bindings & Optimization (1-2 weeks)
**Deliverables:**
- pybind11 bindings
- NumPy integration
- Performance profiling and optimization
- Documentation and examples
- Integration with PyTorch/JAX example

**Success Criteria:**
- Zero-copy for large arrays
- < 10% overhead vs native C++ API
- Easy to use from Python training loops

### Phase 5: Advanced Features (Optional)
**Deliverables:**
- N-step return support
- Trajectory storage mode
- Image compression (optional)
- Monitoring/metrics interface
- Persistence and recovery

## API Design

### C++ API (Preliminary)

```cpp
// Configuration
struct BufferConfig {
    size_t capacity;
    SamplingStrategy strategy;  // Uniform, Prioritized
    float alpha;  // Priority exponent
    float beta;   // IS weight exponent
    bool enable_compression;
};

// Main interface
template<typename Observation, typename Action>
class ReplayBuffer {
public:
    ReplayBuffer(const BufferConfig& config);
    
    // Add transition
    void add(const Transition<Observation, Action>& transition);
    
    // Sample batch
    std::vector<Transition<Observation, Action>> 
        sample(size_t batch_size);
    
    // Update priorities (for prioritized replay)
    void update_priorities(const std::vector<size_t>& indices, 
                          const std::vector<float>& priorities);
    
    // Query
    size_t size() const;
    size_t capacity() const;
    bool is_full() const;
};
```

### Python API (Preliminary)

```python
from replay_buffer import ReplayBuffer, BufferConfig

# Initialize
config = BufferConfig(
    capacity=1_000_000,
    strategy="prioritized",
    alpha=0.6,
    beta=0.4
)
buffer = ReplayBuffer(config, obs_shape=(84, 84, 4), action_dim=6)

# Add transition
buffer.add(obs, action, reward, next_obs, done)

# Sample batch (returns dict of numpy arrays)
batch = buffer.sample(batch_size=32)
# batch = {'obs': ndarray, 'action': ndarray, 'reward': ndarray, ...}

# Update priorities after computing TD errors
buffer.update_priorities(indices, td_errors)
```

## Testing Strategy

### Unit Tests
- Circular buffer wraparound correctness
- Sampling distribution verification
- Thread safety (concurrent adds and samples)
- Priority update correctness
- Edge cases (empty buffer, single element, etc.)

### Integration Tests
- Multi-process reader/writer scenarios
- Python binding integration
- Memory leak detection (valgrind/ASAN)
- Stress tests (sustained high throughput)

### Benchmarks
- Throughput (adds/samples per second)
- Latency (p50, p99 for operations)
- Memory usage vs buffer size
- Multi-process overhead
- Comparison with existing implementations (if available)

## Performance Targets

- **Add operation**: < 100ns (single-threaded)
- **Sample operation**: < 1μs for batch of 32 (uniform)
- **Sample operation**: < 10μs for batch of 32 (prioritized)
- **Throughput**: > 1M transitions/sec (single writer)
- **Memory overhead**: < 10% beyond raw data size

## Dependencies

### Required
- CMake 3.20+
- C++20 compliant compiler (Clang on macOS)
- pybind11
- Google Test or Catch2

### Optional
- boost::interprocess (for advanced IPC)
- benchmark library (Google Benchmark)
- LZ4 or Zstd (for compression)

## Success Metrics

### Technical
- All unit tests passing
- Performance targets met
- Clean valgrind/ASAN runs
- Successfully integrated with a real training loop

### Learning
- Comfortable with modern C++ features
- Understanding of lock-free programming
- Experience with IPC mechanisms
- Portfolio-worthy project with clean documentation

## Future Extensions

- Distributed buffer across multiple machines
- GPU-direct memory access for zero-copy to device
- Advanced sampling (CER, HER, etc.)
- Integration with popular RL libraries
- Web dashboard for monitoring buffer statistics

## Documentation Plan

- README with quick start
- API documentation (Doxygen)
- Architecture overview with diagrams
- Performance tuning guide
- Example usage with PyTorch DQN implementation
- Blog post documenting key learnings

## Timeline Estimate

- **Total**: 6-10 weeks (part-time)
- **MVP** (Phases 1-2): 3-5 weeks
- **Production Ready** (Phases 1-4): 6-8 weeks
- **Feature Complete** (All phases): 8-10 weeks

## Resources

- [Prioritized Experience Replay Paper](https://arxiv.org/abs/1511.05952)
- [Sum Tree Data Structure](https://adventuresinmachinelearning.com/sumtree-introduction-python/)
- C++20/23 documentation and best practices
- POSIX shared memory documentation
- Existing implementations for reference (OpenAI Baselines, RLlib)