#pragma once

/// @file sum_tree.h
/// @brief Sum-tree data structure for efficient proportional sampling.
///

#include <stdexcept>
#include <vector>

namespace replay_buffer {
class SumTree {
  /// @brief Sum tree implementation using 0-indexed convention for array-heap
  /// arithmetic. There are ~2N nodes in total. N leaf nodes and N-1 internal
  /// nodes (sum nodes).
  /// Root is stored at index 0 and leaves start at index capacity - 1.
  /// For leaf at leaves[i]: tree_index = capacity - 1 + i. Parent of node i:
  /// (i - 1) / 2. Left child of node i: 2*i + 1. Right child of node i: 2*i +
  /// 2.
 public:
  explicit SumTree(size_t capacity) {
    if (capacity == 0) {
      throw std::invalid_argument("Capacity must be greater than 0");
    }
    capacity_ = capacity;
    tree_.resize(2 * capacity_);
  }

  size_t capacity() const { return capacity_; }

  void set(size_t index, float priority) {
    if (index >= capacity_) {
      throw std::out_of_range("Index out of range");
    }
    const float existing_priority = tree_[capacity_ - 1 + index];
    const float priority_delta = priority - existing_priority;
    tree_[capacity_ - 1 + index] = priority;
    // start at the leaf and walk back up the parents and update the priority
    // sum by the delta
    size_t tree_index = capacity_ - 1 + index;
    while (tree_index > 0) {
      tree_index = (tree_index - 1) / 2;
      tree_[tree_index] += priority_delta;
    }
  }

  float get(size_t index) {
    if (index >= capacity_) {
      throw std::out_of_range("Index out of range");
    }
    return tree_[capacity_ - 1 + index];
  }

  float total() const { return tree_[0]; }

  size_t sample(float value) {
    if (value < 0 || value > tree_[0]) {
      throw std::out_of_range("Sample value out of range [0, total]");
    }
    size_t index = 0;
    while (index < capacity_ - 1) {
      if (value < tree_[2 * index + 1]) {
        // go left
        index = 2 * index + 1;
      } else {
        // subtract the left child from the value and go right
        value -= tree_[2 * index + 1];
        index = 2 * index + 2;
      }
    }
    return index - capacity_ + 1;
  }

 private:
  size_t capacity_;
  std::vector<float> tree_;
};
}  // namespace replay_buffer
