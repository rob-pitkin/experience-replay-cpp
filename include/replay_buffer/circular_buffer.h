#pragma once

/// @file circular_buffer.h
/// @brief Fixed-capacity circular buffer with automatic wraparound.
/// When the buffer is full, new additions overwrite the oldest elements.
/// Provides O(1) add and access operations.

#include <cstddef>
#include <shared_mutex>
#include <stdexcept>
#include <vector>

namespace replay_buffer {
/// @brief Fixed-capacity circular buffer with automatic wraparound.
/// When the buffer is full, new additions overwrite the oldest elements.
/// Provides O(1) add and access operations.
/// @tparam T Type of elements stored in the buffer
template <typename T>
class CircularBuffer {
 public:
  explicit CircularBuffer(size_t capacity) {
    if (capacity == 0) {
      throw std::invalid_argument("Capacity must be greater than 0");
    }
    capacity_ = capacity;
    size_ = 0;
    buffer_.resize(capacity_);
    head_ = 0;
    tail_ = 0;
  }

  size_t size() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return size_;
  }

  size_t capacity() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return capacity_;
  }

  bool is_full() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return size_ == capacity_;
  }

  bool is_empty() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return size_ == 0;
  }

  void add(const T& item) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    // Case 1: Buffer is not full, insert at tail and increment tail
    if (size_ != capacity_) {
      buffer_[tail_] = item;
      tail_ = (tail_ + 1) % capacity_;
      size_++;
    }
    // Case 2: Buffer is full, overwrite oldest element and increment tail and
    // head
    else {
      buffer_[tail_] = item;
      tail_ = (tail_ + 1) % capacity_;
      head_ = (head_ + 1) % capacity_;
    }
  }

  void clear() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    buffer_.clear();
    size_ = 0;
    head_ = 0;
    tail_ = 0;
  }

  T& operator[](size_t index) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (index >= size_) {
      throw std::out_of_range("Index out of range");
    }
    return buffer_[(head_ + index) % capacity_];
  }

  const T& operator[](size_t index) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (index >= size_) {
      throw std::out_of_range("Index out of range");
    }
    return buffer_[(head_ + index) % capacity_];
  }

  T& at(size_t index) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (index >= size_) {
      throw std::out_of_range("Index out of range");
    }
    return buffer_[(head_ + index) % capacity_];
  }

  const T& at(size_t index) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (index >= size_) {
      throw std::out_of_range("Index out of range");
    }
    return buffer_[(head_ + index) % capacity_];
  }

 private:
  size_t capacity_;
  std::vector<T> buffer_;
  size_t size_;
  size_t head_;
  size_t tail_;
  mutable std::shared_mutex mutex_;
};
}  // namespace replay_buffer
