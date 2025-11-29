#include <benchmark/benchmark.h>
#include <replay_buffer/prioritized_replay_buffer.h>

#include "replay_buffer/transition.h"

static void BM_PrioritizedReplayBufferAdd(benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);

  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = buffer_size;

  static replay_buffer::PrioritizedReplayBuffer<
      replay_buffer::Transition<int, int>>
      buffer(config);

  for (int i = 0; i < buffer_size; ++i) {
    replay_buffer::Transition<int, int> transition(i, i, 1.0f, i, false);
    buffer.add(transition);
  }

  for (auto run : state) {
    replay_buffer::Transition<int, int> transition(state.thread_index(),
                                                   state.thread_index(), 1.0f,
                                                   state.thread_index(), false);
    buffer.add(transition);
  }
}

static void BM_PrioritizedReplayBufferConcurrentAdd(benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);

  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = buffer_size;

  static replay_buffer::PrioritizedReplayBuffer<
      replay_buffer::Transition<int, int>>
      buffer(config);

  if (state.thread_index() == 0) {
    for (int i = 0; i < buffer_size; ++i) {
      replay_buffer::Transition<int, int> transition(i, i, 1.0f, i, false);
      buffer.add(transition);
    }
  }

  for (auto run : state) {
    replay_buffer::Transition<int, int> transition(state.thread_index(),
                                                   state.thread_index(), 1.0f,
                                                   state.thread_index(), false);
    buffer.add(transition);
  }
}

static void BM_PrioritizedReplayBufferSample(benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);
  static bool initialized = false;

  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = buffer_size;

  static replay_buffer::PrioritizedReplayBuffer<
      replay_buffer::Transition<int, int>>
      buffer(config);

  if (!initialized) {
    for (int i = 0; i < buffer_size; ++i) {
      replay_buffer::Transition<int, int> transition(
          i, i, static_cast<float>(i), i, false);
      buffer.add(transition);
    }

    // Create diverse priorities
    std::vector<size_t> indices;
    std::vector<float> td_errors;
    for (int i = 0; i < buffer_size; ++i) {
      indices.push_back(i);
      td_errors.push_back(static_cast<float>(i % 10 + 1));
    }
    buffer.update_priorities(indices, td_errors);
    initialized = true;
  }

  for (auto run : state) {
    buffer.sample(32);
  }
}

static void BM_PrioritizedReplayBufferConcurrentSample(
    benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);

  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = buffer_size;

  static replay_buffer::PrioritizedReplayBuffer<
      replay_buffer::Transition<int, int>>
      buffer(config);

  static bool initialized = false;

  if (!initialized) {
    if (state.thread_index() == 0) {
      for (int i = 0; i < buffer_size; ++i) {
        replay_buffer::Transition<int, int> transition(
            i, i, static_cast<float>(i), i, false);
        buffer.add(transition);
      }

      // Create diverse priorities
      std::vector<size_t> indices;
      std::vector<float> td_errors;
      for (int i = 0; i < buffer_size; ++i) {
        indices.push_back(i);
        td_errors.push_back(static_cast<float>(i % 10 + 1));
      }
      buffer.update_priorities(indices, td_errors);
      initialized = true;
    }
  }

  for (auto run : state) {
    buffer.sample(32);
  }
}

static void BM_PrioritizedReplayBufferUpdatePriorities(
    benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);
  static bool initialized = false;

  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = buffer_size;

  static replay_buffer::PrioritizedReplayBuffer<
      replay_buffer::Transition<int, int>>
      buffer(config);

  static std::vector<size_t> update_indices;
  static std::vector<float> update_td_errors;

  if (!initialized) {
    for (int i = 0; i < buffer_size; ++i) {
      replay_buffer::Transition<int, int> transition(
          i, i, static_cast<float>(i), i, false);
      buffer.add(transition);
    }

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, buffer_size - 1);
    std::uniform_real_distribution<float> td_dist(0.1f, 10.0f);
    for (int i = 0; i < 32; ++i) {
      update_indices.push_back(dist(gen));
      update_td_errors.push_back(td_dist(gen));
    }
    initialized = true;
  }

  for (auto run : state) {
    buffer.update_priorities(update_indices, update_td_errors);
  }
}

static void BM_PrioritizedReplayBufferConcurrentUpdatePriorities(
    benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);
  static bool initialized = false;

  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = buffer_size;

  static replay_buffer::PrioritizedReplayBuffer<
      replay_buffer::Transition<int, int>>
      buffer(config);

  static std::vector<size_t> update_indices;
  static std::vector<float> update_td_errors;

  if (!initialized) {
    if (state.thread_index() == 0) {
      for (int i = 0; i < buffer_size; ++i) {
        replay_buffer::Transition<int, int> transition(
            i, i, static_cast<float>(i), i, false);
        buffer.add(transition);
      }

      std::mt19937 gen(std::random_device{}());
      std::uniform_int_distribution<size_t> dist(0, buffer_size - 1);
      std::uniform_real_distribution<float> td_dist(0.1f, 10.0f);
      for (int i = 0; i < 32; ++i) {
        update_indices.push_back(dist(gen));
        update_td_errors.push_back(td_dist(gen));
      }

      initialized = true;
    }
  }

  for (auto run : state) {
    buffer.update_priorities(update_indices, update_td_errors);
  }
}

BENCHMARK(BM_PrioritizedReplayBufferAdd)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_PrioritizedReplayBufferConcurrentAdd)
    ->ThreadRange(1, 8)
    ->Arg(1000)
    ->Arg(100000)
    ->Arg(1000000);
BENCHMARK(BM_PrioritizedReplayBufferSample)
    ->Arg(1000)
    ->Arg(100000)
    ->Arg(1000000);
BENCHMARK(BM_PrioritizedReplayBufferConcurrentSample)
    ->ThreadRange(1, 8)
    ->Arg(1000)
    ->Arg(100000)
    ->Arg(1000000);
BENCHMARK(BM_PrioritizedReplayBufferUpdatePriorities)
    ->Arg(1000)
    ->Arg(100000)
    ->Arg(1000000);
BENCHMARK(BM_PrioritizedReplayBufferConcurrentUpdatePriorities)
    ->ThreadRange(1, 8)
    ->Arg(1000)
    ->Arg(100000)
    ->Arg(1000000);
