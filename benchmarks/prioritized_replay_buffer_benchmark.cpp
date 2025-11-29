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

  replay_buffer::PrioritizedReplayBuffer<replay_buffer::Transition<int, int>>
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

BENCHMARK(BM_PrioritizedReplayBufferAdd)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_PrioritizedReplayBufferConcurrentAdd)
    ->ThreadRange(1, 8)
    ->Arg(1000)
    ->Arg(100000)
    ->Arg(1000000);
