#include <cmath>
#include <random>
#include <shared_mutex>
#include <vector>

#include "replay_buffer/circular_buffer.h"
#include "replay_buffer/sum_tree.h"

namespace replay_buffer {

template <typename T>
struct PrioritizedSample {
  T transition;
  float weight;
  size_t index;
};

struct PrioritizedReplayBufferConfig {
  size_t capacity;
  float alpha = 0.6f;
  float beta = 0.4f;
  float epsilon = 1e-6f;
};

template <typename T>
class PrioritizedReplayBuffer {
 public:
  PrioritizedReplayBuffer(const PrioritizedReplayBufferConfig& config)
      : buffer_(config.capacity),
        tree_(config.capacity),
        capacity_(config.capacity),
        alpha_(config.alpha),
        beta_(config.beta),
        epsilon_(config.epsilon),
        max_priority_(1.0f),
        gen_(std::random_device{}()) {
    if (config.capacity == 0) {
      throw std::invalid_argument("Capacity must be greater than 0");
    }
    if (config.alpha < 0.0f || config.alpha > 1.0f) {
      throw std::invalid_argument("Alpha must be between 0 and 1");
    }
    if (config.beta < 0.0f || config.beta > 1.0f) {
      throw std::invalid_argument("Beta must be between 0 and 1");
    }
    if (config.epsilon < 0.0f) {
      throw std::invalid_argument("Epsilon must be non-negative");
    }
  }

  size_t capacity() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return capacity_;
  }

  size_t size() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return buffer_.size();
  }

  float alpha() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return alpha_;
  }

  float beta() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return beta_;
  }

  float epsilon() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return epsilon_;
  }

  void add(const T& item) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    size_t stored_index = buffer_.add(item);
    tree_.set(stored_index, max_priority_);
  }

  std::vector<replay_buffer::PrioritizedSample<T>> sample(
      size_t batch_size) const {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    std::vector<replay_buffer::PrioritizedSample<T>> samples;
    std::uniform_real_distribution<float> dist(0.0f, tree_.total());
    for (size_t i = 0; i < batch_size; i++) {
      float random_value = dist(gen_);
      size_t index = tree_.sample(random_value);
      float importance_sampling_weight =
          std::pow(buffer_.size() * (tree_.get(index) / tree_.total()), -beta_);
      samples.push_back(replay_buffer::PrioritizedSample<T>{
          buffer_[index], importance_sampling_weight, index});
    }

    return samples;
  }

  void update_priorities(const std::vector<size_t>& indices,
                         const std::vector<float>& td_errors) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    for (size_t i = 0; i < indices.size(); i++) {
      float priority = std::pow(std::abs(td_errors[i]) + epsilon_, alpha_);
      tree_.set(indices[i], priority);
      if (max_priority_ < priority) {
        max_priority_ = priority;
      }
    }
  }

 private:
  replay_buffer::CircularBuffer<T> buffer_;
  replay_buffer::SumTree tree_;
  mutable std::shared_mutex mutex_;
  size_t capacity_;
  float alpha_;
  float beta_;
  float epsilon_;
  float max_priority_;
  mutable std::mt19937 gen_;
};
}  // namespace replay_buffer
