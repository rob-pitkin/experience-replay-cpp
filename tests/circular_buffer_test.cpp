#include <gtest/gtest.h>
#include <replay_buffer/circular_buffer.h>
#include <replay_buffer/transition.h>

#include <string>
#include <thread>

TEST(ConstructionTest, ValidCapacity) {
  replay_buffer::CircularBuffer<int> buffer(10);

  // Verify the capacity and size are correct
  EXPECT_EQ(buffer.capacity(), 10);
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_TRUE(buffer.is_empty());
  EXPECT_FALSE(buffer.is_full());
}

TEST(ConstructionTest, ZeroCapacityThrows) {
  EXPECT_THROW(
      { replay_buffer::CircularBuffer<int> buf(0); }, std::invalid_argument);
}

TEST(AddTest, AddsElementsWhenNotFull) {
  replay_buffer::CircularBuffer<int> buffer(5);

  // Add 3 elements when not full
  const size_t index_1 = buffer.add(1);
  const size_t index_2 = buffer.add(2);
  const size_t index_3 = buffer.add(3);

  // Verify the size, emptiness, and contents
  EXPECT_EQ(buffer.size(), 3);
  EXPECT_FALSE(buffer.is_full());
  EXPECT_FALSE(buffer.is_empty());
  EXPECT_EQ(buffer[0], 1);
  EXPECT_EQ(buffer[1], 2);
  EXPECT_EQ(buffer[2], 3);
  EXPECT_EQ(index_1, 0);
  EXPECT_EQ(index_2, 1);
  EXPECT_EQ(index_3, 2);
  EXPECT_EQ(buffer.at(0), 1);
  EXPECT_EQ(buffer.at(1), 2);
  EXPECT_EQ(buffer.at(2), 3);
}

TEST(AddTest, FillsToCapacity) {
  replay_buffer::CircularBuffer<int> buffer(5);

  // Add 5 elements to fill the buffer
  const size_t index_1 = buffer.add(1);
  const size_t index_2 = buffer.add(2);
  const size_t index_3 = buffer.add(3);
  const size_t index_4 = buffer.add(4);
  const size_t index_5 = buffer.add(5);

  // Verify the size, fullness, and contents
  EXPECT_EQ(buffer.size(), 5);
  EXPECT_TRUE(buffer.is_full());
  EXPECT_FALSE(buffer.is_empty());
  EXPECT_EQ(buffer[0], 1);
  EXPECT_EQ(buffer[1], 2);
  EXPECT_EQ(buffer[2], 3);
  EXPECT_EQ(buffer[3], 4);
  EXPECT_EQ(buffer[4], 5);
  EXPECT_EQ(index_1, 0);
  EXPECT_EQ(index_2, 1);
  EXPECT_EQ(index_3, 2);
  EXPECT_EQ(index_4, 3);
  EXPECT_EQ(index_5, 4);
  EXPECT_EQ(buffer.at(0), 1);
  EXPECT_EQ(buffer.at(1), 2);
  EXPECT_EQ(buffer.at(2), 3);
  EXPECT_EQ(buffer.at(3), 4);
  EXPECT_EQ(buffer.at(4), 5);
}

TEST(AddTest, WrapsAroundWhenFull) {
  replay_buffer::CircularBuffer<int> buffer(3);
  // Add 3 elements to fill the buffer
  const size_t index_1 = buffer.add(1);
  const size_t index_2 = buffer.add(2);
  const size_t index_3 = buffer.add(3);
  // Add 4th element to wrap around
  const size_t index_4 = buffer.add(4);
  // Verify the size, fullness, and contents
  EXPECT_EQ(buffer.size(), 3);
  EXPECT_TRUE(buffer.is_full());
  EXPECT_EQ(buffer[0], 2);
  EXPECT_EQ(buffer[1], 3);
  EXPECT_EQ(buffer[2], 4);
  EXPECT_EQ(buffer[buffer.size() - 1], 4);
  EXPECT_EQ(buffer.at(0), 2);
  EXPECT_EQ(buffer.at(1), 3);
  EXPECT_EQ(buffer.at(2), 4);
  EXPECT_EQ(buffer.at(buffer.size() - 1), 4);
  EXPECT_EQ(index_1, 0);
  EXPECT_EQ(index_2, 1);
  EXPECT_EQ(index_3, 2);
  EXPECT_EQ(index_4, 0);
}

TEST(AccessTest, OperatorBracket) {
  replay_buffer::CircularBuffer<int> buffer(3);
  buffer.add(1);
  buffer.add(2);
  buffer.add(3);
  EXPECT_EQ(buffer[0], 1);
  EXPECT_EQ(buffer[1], 2);
  EXPECT_EQ(buffer[2], 3);
  EXPECT_EQ(buffer[buffer.size() - 1], 3);
  EXPECT_EQ(buffer.at(0), 1);
  EXPECT_EQ(buffer.at(1), 2);
  EXPECT_EQ(buffer.at(2), 3);
  EXPECT_EQ(buffer.at(buffer.size() - 1), 3);
}

TEST(AccessTest, OutOfBoundsThrows) {
  replay_buffer::CircularBuffer<int> buffer(3);
  buffer.add(7);
  buffer.add(8);
  buffer.add(9);
  EXPECT_THROW(buffer[3], std::out_of_range);
  EXPECT_THROW(buffer.at(3), std::out_of_range);
}

TEST(AccessTest, ConstAccess) {
  replay_buffer::CircularBuffer<int> buffer(3);
  buffer.add(7);
  buffer.add(8);
  buffer.add(9);

  const replay_buffer::CircularBuffer<int>& const_buffer_ref = buffer;
  EXPECT_EQ(const_buffer_ref[0], 7);
  EXPECT_EQ(const_buffer_ref[1], 8);
  EXPECT_EQ(const_buffer_ref[2], 9);
  EXPECT_EQ(const_buffer_ref.at(0), 7);
  EXPECT_EQ(const_buffer_ref.at(1), 8);
  EXPECT_EQ(const_buffer_ref.at(2), 9);
}

TEST(ClearTest, RemovesAllElements) {
  replay_buffer::CircularBuffer<int> buffer(3);
  buffer.add(7);
  buffer.add(8);
  buffer.add(9);
  buffer.clear();
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_TRUE(buffer.is_empty());
  EXPECT_FALSE(buffer.is_full());
  EXPECT_THROW(buffer[0], std::out_of_range);
  EXPECT_THROW(buffer.at(0), std::out_of_range);
}

TEST(WrapAroundTest, MultipleWraps) {
  replay_buffer::CircularBuffer<int> buffer(3);
  for (size_t i = 0; i < 10; i++) {
    buffer.add(i + 1);
  }
  EXPECT_EQ(buffer.size(), 3);
  EXPECT_TRUE(buffer.is_full());
  EXPECT_FALSE(buffer.is_empty());
  EXPECT_EQ(buffer[0], 8);
  EXPECT_EQ(buffer[1], 9);
  EXPECT_EQ(buffer[2], 10);
  EXPECT_EQ(buffer.at(0), 8);
  EXPECT_EQ(buffer.at(1), 9);
  EXPECT_EQ(buffer.at(2), 10);
}

TEST(TemplateTest, WorksWithDifferentTypes) {
  replay_buffer::CircularBuffer<float> float_buffer(3);
  float_buffer.add(1.1f);
  float_buffer.add(2.2f);
  float_buffer.add(3.3f);
  EXPECT_EQ(float_buffer.size(), 3);
  EXPECT_TRUE(float_buffer.is_full());
  EXPECT_FALSE(float_buffer.is_empty());
  EXPECT_EQ(float_buffer[0], 1.1f);
  EXPECT_EQ(float_buffer[1], 2.2f);
  EXPECT_EQ(float_buffer[2], 3.3f);
  EXPECT_EQ(float_buffer.at(0), 1.1f);
  EXPECT_EQ(float_buffer.at(1), 2.2f);
  EXPECT_EQ(float_buffer.at(2), 3.3f);

  replay_buffer::CircularBuffer<std::string> string_buffer(3);
  string_buffer.add("hello");
  string_buffer.add("world");
  string_buffer.add("!");
  EXPECT_EQ(string_buffer.size(), 3);
  EXPECT_TRUE(string_buffer.is_full());
  EXPECT_FALSE(string_buffer.is_empty());
  EXPECT_EQ(string_buffer[0], "hello");
  EXPECT_EQ(string_buffer[1], "world");
  EXPECT_EQ(string_buffer[2], "!");
  EXPECT_EQ(string_buffer.at(0), "hello");
  EXPECT_EQ(string_buffer.at(1), "world");
  EXPECT_EQ(string_buffer.at(2), "!");

  replay_buffer::CircularBuffer<replay_buffer::Transition<int, int>>
      transition_buffer(3);
  transition_buffer.add({1, 2, 3.0f, 4, false});
  transition_buffer.add({5, 6, 7.0f, 8, true});
  transition_buffer.add({9, 10, 11.0f, 12, false});
  EXPECT_EQ(transition_buffer.size(), 3);
  EXPECT_TRUE(transition_buffer.is_full());
  EXPECT_FALSE(transition_buffer.is_empty());
  EXPECT_EQ(transition_buffer[0].observation, 1);
  EXPECT_EQ(transition_buffer[0].action, 2);
  EXPECT_EQ(transition_buffer[0].reward, 3.0f);
  EXPECT_EQ(transition_buffer[0].next_observation, 4);
  EXPECT_EQ(transition_buffer[0].done, false);
  EXPECT_EQ(transition_buffer[1].observation, 5);
  EXPECT_EQ(transition_buffer[1].action, 6);
  EXPECT_EQ(transition_buffer[1].reward, 7.0f);
  EXPECT_EQ(transition_buffer[1].next_observation, 8);
  EXPECT_EQ(transition_buffer[1].done, true);
  EXPECT_EQ(transition_buffer[2].observation, 9);
  EXPECT_EQ(transition_buffer[2].action, 10);
  EXPECT_EQ(transition_buffer[2].reward, 11.0f);
  EXPECT_EQ(transition_buffer[2].next_observation, 12);
  EXPECT_EQ(transition_buffer[2].done, false);
}

TEST(ThreadSafetyTest, ConcurrentAdds) {
  replay_buffer::CircularBuffer<int> buffer(1000);

  std::vector<std::thread> threads;
  // Create 10 threads, each adding 100 elements, starting from 1 to 1000
  for (int t = 0; t < 10; ++t) {
    threads.emplace_back([&buffer, t]() {
      for (int i = 0; i < 100; ++i) {
        buffer.add(t * 100 + i);
      }
    });
  }

  for (std::thread& thread : threads) {
    thread.join();
  }

  EXPECT_EQ(buffer.size(), 1000);
  EXPECT_TRUE(buffer.is_full());
}

TEST(ThreadSafetyTest, ConcurrentReadsAndWrites) {
  replay_buffer::CircularBuffer<int> buffer(100);

  for (int i = 0; i < 100; ++i) {
    buffer.add(1);
  }

  std::atomic<bool> stop(false);
  std::atomic<int> read_count(0);

  std::thread writer([&]() {
    int value = 100;
    while (!stop.load()) {
      buffer.add(value++);
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  });

  std::vector<std::thread> readers;
  for (int i = 0; i < 5; i++) {
    readers.emplace_back([&]() {
      while (!stop.load()) {
        size_t size = buffer.size();
        if (size > 0) {
          int value = buffer[size / 2];
          (void)value;
          read_count++;
        }
      }
    });
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  stop.store(true);

  writer.join();
  for (std::thread& reader : readers) {
    reader.join();
  }

  EXPECT_GT(read_count.load(), 0);
  std::cout << "Completed " << read_count.load() << " concurrent reads"
            << std::endl;
}

TEST(ThreadSafetyTest, ConcurrentQueries) {
  replay_buffer::CircularBuffer<int> buffer(50);

  for (int i = 0; i < 25; ++i) {
    buffer.add(i);
  }

  std::atomic<bool> stop(false);

  std::vector<std::thread> threads;
  for (int i = 0; i < 5; ++i) {
    threads.emplace_back([&]() {
      while (!stop.load()) {
        EXPECT_GE(buffer.size(), 0);
        EXPECT_LE(buffer.size(), buffer.capacity());
        EXPECT_EQ(buffer.capacity(), 50);

        if (!buffer.is_empty()) {
          EXPECT_GT(buffer.size(), 0);
        }
        if (buffer.is_full()) {
          EXPECT_EQ(buffer.size(), buffer.capacity());
        }
      }
    });
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  stop.store(true);

  for (std::thread& thread : threads) {
    thread.join();
  }
}

TEST(SamplingTest, SamplesCorrectBatchSize) {
  replay_buffer::CircularBuffer<int> buffer(100);

  for (int i = 0; i < 50; ++i) {
    buffer.add(i);
  }

  std::vector<int> batch = buffer.sample(10);

  EXPECT_EQ(batch.size(), 10);
}

TEST(SamplingTest, ThrowsIfBatchSizeTooLarge) {
  replay_buffer::CircularBuffer<int> buffer(3);
  buffer.add(1);
  buffer.add(2);
  buffer.add(3);

  EXPECT_THROW(buffer.sample(4), std::invalid_argument);
}

TEST(SamplingTest, ThrowsIfBatchSizeZero) {
  replay_buffer::CircularBuffer<int> buffer(3);
  buffer.add(1);
  buffer.add(2);
  buffer.add(3);

  EXPECT_THROW(buffer.sample(0), std::invalid_argument);
}

TEST(SamplingTest, ReturnsValidElements) {
  replay_buffer::CircularBuffer<int> buffer(10);

  for (int i = 10; i < 20; ++i) {
    buffer.add(i);
  }

  std::vector<int> batch = buffer.sample(5);
  EXPECT_EQ(batch.size(), 5);
  for (const int& elem : batch) {
    EXPECT_GE(elem, 10);
    EXPECT_LT(elem, 20);
  }
}

TEST(SamplingTest, WorksAfterWraparound) {
  replay_buffer::CircularBuffer<int> buffer(5);

  for (int i = 0; i < 10; ++i) {
    buffer.add(i);
  }

  std::vector<int> batch = buffer.sample(5);

  for (const int& elem : batch) {
    EXPECT_GE(elem, 5);
    EXPECT_LT(elem, 10);
  }
}

TEST(SamplingTest, UniformDistribution) {
  replay_buffer::CircularBuffer<int> buffer(10);

  for (int i = 0; i < 10; ++i) {
    buffer.add(i);
  }

  std::vector<int> counts(10, 0);
  const int num_samples = 10000;

  for (int i = 0; i < num_samples; ++i) {
    std::vector<int> batch = buffer.sample(1);
    counts[batch[0]]++;
  }

  for (int i = 0; i < 10; ++i) {
    EXPECT_GT(counts[i], 700);
    EXPECT_LT(counts[i], 1300);
  }
}

TEST(SamplingTest, ConcurrentSampling) {
  replay_buffer::CircularBuffer<int> buffer(1000);

  // Fill buffer
  for (int i = 0; i < 1000; ++i) {
    buffer.add(i);
  }

  std::vector<std::thread> threads(10);
  std::atomic<int> successful_samples(0);

  for (int t = 0; t < 10; ++t) {
    threads[t] = std::thread([&]() {
      for (int i = 0; i < 100; ++i) {
        std::vector<int> batch = buffer.sample(32);
        if (batch.size() == 32) {
          successful_samples++;
        }
      }
    });
  }

  for (std::thread& thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successful_samples, 1000);
}

TEST(SamplingTest, WorksWithTransitions) {
  using Transition = replay_buffer::Transition<int, int>;
  replay_buffer::CircularBuffer<Transition> buffer(10);

  for (int i = 0; i < 10; ++i) {
    buffer.add({i, i * 2, float(i), i + 1, false});
  }

  std::vector<Transition> batch = buffer.sample(5);
  EXPECT_EQ(batch.size(), 5);

  for (const Transition& t : batch) {
    EXPECT_EQ(t.action, t.observation * 2);
    EXPECT_EQ(t.reward, float(t.observation));
    EXPECT_EQ(t.next_observation, t.observation + 1);
    EXPECT_EQ(t.done, false);
  }
}
