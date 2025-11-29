#include <benchmark/benchmark.h>
#include <replay_buffer/circular_buffer.h>

static void BM_CircularBufferAdd(benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);
  replay_buffer::CircularBuffer<int> buffer(buffer_size);

  for (int i = 0; i < buffer_size; ++i) {
    buffer.add(i);
  }

  for (auto run : state) {
    buffer.add(1);
  }
}

static void BM_CircularBufferSample(benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);
  replay_buffer::CircularBuffer<int> buffer(buffer_size);

  for (int i = 0; i < buffer_size; ++i) {
    buffer.add(i);
  }

  for (auto run : state) {
    buffer.sample(32);
  }
}

static void BM_CircularBufferConcurrentAdd(benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);
  static replay_buffer::CircularBuffer<int> buffer(buffer_size);

  if (state.thread_index() == 0) {
    for (int i = 0; i < buffer_size; ++i) {
      buffer.add(i);
    }
  }

  for (auto run : state) {
    buffer.add(state.thread_index() + 1);
  }
}

static void BM_CircularBufferConcurrentSample(benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);
  static replay_buffer::CircularBuffer<int> buffer(buffer_size);

  if (state.thread_index() == 0) {
    for (int i = 0; i < buffer_size; ++i) {
      buffer.add(i);
    }
  }

  for (auto run : state) {
    buffer.sample(32);
  }
}

static void BM_CircularBufferConcurrentRead(benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);
  static replay_buffer::CircularBuffer<int> buffer(buffer_size);

  // Prefill buffer
  if (state.thread_index() == 0) {
    for (int i = 0; i < buffer_size; ++i) {
      buffer.add(i);
    }
  }

  // One thread writes, all threads read
  for (auto run : state) {
    buffer[state.thread_index()];
  }
}

static void BM_CircularBufferConcurrentWriteAndRead(benchmark::State& state) {
  // access first parameter
  int buffer_size = state.range(0);
  static replay_buffer::CircularBuffer<int> buffer(buffer_size);

  // Prefill buffer
  if (state.thread_index() == 0) {
    for (int i = 0; i < buffer_size; ++i) {
      buffer.add(i);
    }
  }

  // One thread writes, all threads read
  for (auto run : state) {
    if (state.thread_index() == 0) {
      buffer.add(state.thread_index() + 1);
    } else {
      buffer[state.thread_index()];
    }
  }
}

BENCHMARK(BM_CircularBufferAdd)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_CircularBufferSample)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_CircularBufferConcurrentAdd)
    ->ThreadRange(1, 8)
    ->Arg(1000)
    ->Arg(100000)
    ->Arg(1000000);

BENCHMARK(BM_CircularBufferConcurrentSample)
    ->ThreadRange(1, 8)
    ->Arg(1000)
    ->Arg(100000)
    ->Arg(1000000);

BENCHMARK(BM_CircularBufferConcurrentRead)
    ->ThreadRange(1, 8)
    ->Arg(1000)
    ->Arg(100000)
    ->Arg(1000000);

BENCHMARK(BM_CircularBufferConcurrentWriteAndRead)
    ->ThreadRange(1, 8)
    ->Arg(1000)
    ->Arg(100000)
    ->Arg(1000000);
