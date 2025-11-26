# Prioritized Experience Replay Primer

## What is Prioritized Experience Replay?

**Prioritized Experience Replay (PER)** is a technique from the [Schaul et al. 2015 paper](https://arxiv.org/abs/1511.05952) that improves RL training by sampling transitions non-uniformly based on their **learning potential**.

## The Core Idea

Not all experiences are equally useful for learning:
- Surprising transitions (high TD-error) → more learning signal
- Already well-predicted transitions (low TD-error) → less learning signal

**Solution:** Sample transitions proportionally to their priority (e.g., absolute TD-error)

## How It Works

### 1. Priority Assignment

Each transition gets a priority based on its TD-error:
```
priority_i = |TD-error_i|^α + ε
```

Where:
- **TD-error**: Temporal difference error from last time this transition was used
- **α (alpha)**: Priority exponent, controls how much prioritization
  - α = 0: Uniform sampling (no prioritization)
  - α = 1: Fully proportional to TD-error
  - Typical: α = 0.6
- **ε (epsilon)**: Small constant to ensure all transitions have non-zero probability

### 2. Sampling Probability

Transition `i` gets sampled with probability:
```
P(i) = priority_i / Σ(priority_j)
```

This is **proportional sampling** - implemented efficiently using a sum-tree!

### 3. Importance Sampling Correction

Prioritized sampling introduces bias (not sampling uniformly from experience distribution). We correct this using **importance sampling weights**:
```
weight_i = (N × P(i))^(-β)
```

Where:
- **N**: Buffer size
- **β (beta)**: Importance sampling exponent
  - β = 0: No correction
  - β = 1: Full correction
  - Typical: Start at 0.4, anneal to 1.0

These weights are multiplied with TD-errors during gradient updates to debias learning.

## Architecture Overview

### Components Integration

```
PrioritizedReplayBuffer
├── CircularBuffer<Transition>   ← Stores actual data
│   └── add(), get(), size()
└── SumTree                       ← Stores priorities
    └── set(), sample(), total()
```

**Key invariant:** Index `i` in CircularBuffer corresponds to index `i` in SumTree.

### Operations

#### Adding a Transition
```cpp
1. Add transition to CircularBuffer at index i
2. Set priority in SumTree: sum_tree_.set(i, priority)
   - For new transitions: use max_priority (ensures at least one sample)
   - Gets updated after first training step
```

#### Sampling a Batch
```cpp
1. For each sample in batch:
   a. Generate random value in [0, sum_tree_.total())
   b. Find index: i = sum_tree_.sample(random_value)
   c. Retrieve transition: buffer_[i]
   d. Calculate IS weight: weight_i = (N × P(i))^(-β)
2. Return batch of (transition, weight) pairs
```

#### Updating Priorities
```cpp
1. After training step, compute new TD-errors for sampled transitions
2. For each index i and new priority p:
   sum_tree_.set(i, p^α + ε)
```

## Thread Safety Considerations

### Problem
Two data structures need coordinated updates:
- Adding transition → update both buffer AND tree
- Sampling → read from both buffer AND tree

### Solution: Wrapper-Level Locking
Use `std::shared_mutex` at `PrioritizedReplayBuffer` level:
- **Write operations** (add, update_priorities): exclusive lock
- **Read operations** (sample): shared lock

Neither `CircularBuffer` nor `SumTree` need their own locks - the wrapper coordinates access.

## Implementation Challenges

### Challenge 1: Index Correspondence
**Problem:** CircularBuffer wraps around, but SumTree has fixed indices.

**Solution:**
- CircularBuffer uses modulo arithmetic internally: `buffer_[index % capacity_]`
- PrioritizedReplayBuffer provides logical indices (0 to size-1)
- Wrapper translates between logical indices and physical positions

### Challenge 2: Initial Priorities
**Problem:** New transitions have no TD-error yet.

**Solution:** Use `max_priority_` tracking:
- Keep track of maximum priority seen so far
- New transitions get this maximum (ensures they're sampled at least once)
- Gets updated after first training step

### Challenge 3: Wraparound Handling
**Problem:** When buffer wraps, old transitions get overwritten.

**Solution:**
- When overwriting index `i`, its priority is still in sum-tree
- On next `add()`, update that priority to new value
- No special cleanup needed - priorities get overwritten naturally

## API Design

### Configuration
```cpp
struct PrioritizedBufferConfig {
    size_t capacity;
    float alpha = 0.6f;   // Priority exponent
    float beta = 0.4f;    // IS weight exponent
    float epsilon = 1e-6f; // Small constant
};
```

### Sample Result
```cpp
template<typename T>
struct PrioritizedSample {
    T transition;
    float weight;        // Importance sampling weight
    size_t index;        // Index for priority updates
};
```

### Main Interface
```cpp
template<typename T>
class PrioritizedReplayBuffer {
public:
    void add(const T& transition);
    std::vector<PrioritizedSample<T>> sample(size_t batch_size);
    void update_priorities(const std::vector<size_t>& indices,
                          const std::vector<float>& priorities);
};
```

## Connection to PER Paper

From [Schaul et al. 2015](https://arxiv.org/abs/1511.05952):

### Proportional Prioritization
> "We sample transition i with probability P(i) proportional to its priority p_i."

This is exactly what sum-tree sampling implements!

### Importance Sampling
> "We correct for the bias introduced by prioritized replay using importance sampling weights."

The weights ensure gradient updates remain unbiased despite non-uniform sampling.

### Annealing Beta
> "We exploit the flexibility of annealing the amount of importance-sampling correction over time, by defining a schedule on the exponent β."

Typically: β starts at 0.4, anneals to 1.0 by end of training.

## Performance Characteristics

With correct implementation:
- **Add**: O(log n) - one sum-tree update
- **Sample batch**: O(batch_size × log n) - multiple sum-tree samples
- **Update priorities**: O(batch_size × log n) - multiple sum-tree updates

Compare to naive approach:
- Naive sampling: O(n) per sample
- For batch_size=32 with n=1M: 32M ops vs ~640 ops (50,000× speedup!)

## Testing Strategy

### Distribution Testing
Verify sampling is actually proportional:
1. Set known priorities: [1, 2, 3, 4]
2. Sample 10,000 times
3. Count occurrences of each index
4. Verify approximately: 10%, 20%, 30%, 40% (±5%)

### Edge Cases
- All priorities equal → should behave like uniform sampling
- One very high priority → should be sampled almost every time
- Buffer not full yet → should only sample from valid indices

### Integration Testing
- Add transitions while sampling concurrently (thread safety)
- Update priorities for sampled transitions
- Wraparound behavior (overwrite old transitions)

## Common Pitfalls

### Pitfall 1: Forgetting to Apply Alpha
**Wrong:** Store raw TD-errors in sum-tree
**Correct:** Store `(|TD-error| + ε)^α` in sum-tree

### Pitfall 2: Not Handling Empty Buffer
**Problem:** Sampling from empty buffer crashes
**Solution:** Check `size() > 0` before sampling

### Pitfall 3: Index Mismatch After Wraparound
**Problem:** Logical indices don't match physical positions
**Solution:** Wrapper handles translation consistently

### Pitfall 4: Race Conditions
**Problem:** Concurrent add/sample without proper locking
**Solution:** Use `std::shared_mutex` at wrapper level

## Learning Objectives

By implementing this, you'll understand:
1. **Data structure composition** - combining specialized structures
2. **Sampling algorithms** - discrete probability distributions
3. **Bias correction** - importance sampling in RL
4. **Hyperparameter effects** - how α and β affect learning
5. **Thread-safe composition** - coordinating multiple structures

## Chrome Connection

Chrome's memory allocator uses similar prioritization concepts:
- **Partition allocator** prioritizes certain allocation sizes
- **Task scheduler** prioritizes tasks based on importance
- **Network layer** prioritizes high-priority requests

Key parallel: Efficient sampling from weighted distributions is a common systems problem.

## Next Steps

Your task (ROB-13) is to build `PrioritizedReplayBuffer` that combines your `CircularBuffer` and `SumTree`!

Focus on:
1. Clean integration of both data structures
2. Proper index correspondence
3. Thread-safe coordinated access
4. Correct priority and weight calculations
5. Comprehensive testing of sampling distribution
