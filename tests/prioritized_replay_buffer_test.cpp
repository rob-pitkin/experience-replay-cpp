#include "replay_buffer/prioritized_replay_buffer.h"

#include "gtest/gtest.h"
#include "replay_buffer/transition.h"

TEST(PrioritizedReplayBufferTest, ConstructionTest) {
  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = 10;

  replay_buffer::PrioritizedReplayBuffer<int> buffer(config);

  EXPECT_EQ(buffer.capacity(), 10);
  EXPECT_FLOAT_EQ(buffer.alpha(), 0.6f);
  EXPECT_FLOAT_EQ(buffer.beta(), 0.4f);
  EXPECT_FLOAT_EQ(buffer.epsilon(), 1e-6f);

  config.alpha = 0.1f;
  config.beta = 0.2f;
  config.epsilon = 0.3f;

  replay_buffer::PrioritizedReplayBuffer<int> buffer2(config);

  EXPECT_EQ(buffer2.capacity(), 10);
  EXPECT_FLOAT_EQ(buffer2.alpha(), 0.1f);
  EXPECT_FLOAT_EQ(buffer2.beta(), 0.2f);
  EXPECT_FLOAT_EQ(buffer2.epsilon(), 0.3f);
}

TEST(PrioritizedReplayBufferTest, InvalidConstructionTest) {
  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = 0;
  EXPECT_THROW(replay_buffer::PrioritizedReplayBuffer<int> buffer(config),
               std::invalid_argument);

  config.capacity = 10;
  config.alpha = -1;
  EXPECT_THROW(replay_buffer::PrioritizedReplayBuffer<int> buffer(config),
               std::invalid_argument);

  config.alpha = 1.1;
  EXPECT_THROW(replay_buffer::PrioritizedReplayBuffer<int> buffer(config),
               std::invalid_argument);

  config.alpha = 0.5;
  config.beta = 1.1;
  EXPECT_THROW(replay_buffer::PrioritizedReplayBuffer<int> buffer(config),
               std::invalid_argument);

  config.beta = 0.5;
  config.epsilon = -1;
  EXPECT_THROW(replay_buffer::PrioritizedReplayBuffer<int> buffer(config),
               std::invalid_argument);
}

TEST(PrioritizedReplayBufferTest, AddTransitionsTest) {
  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = 4;
  replay_buffer::PrioritizedReplayBuffer<replay_buffer::Transition<int, int>>
      buffer(config);

  replay_buffer::Transition<int, int> transition(1, 2, 1.0f, 3, false);
  buffer.add(transition);

  EXPECT_EQ(buffer.size(), 1);

  for (size_t i = 0; i < config.capacity; i++) {
    buffer.add(transition);
  }

  EXPECT_EQ(buffer.size(), config.capacity);
}

TEST(PrioritizedReplayBufferTest, SampleTest) {
  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = 4;
  replay_buffer::PrioritizedReplayBuffer<replay_buffer::Transition<int, int>>
      buffer(config);

  replay_buffer::Transition<int, int> transition(1, 2, 1.0f, 3, false);
  buffer.add(transition);

  std::vector<
      replay_buffer::PrioritizedSample<replay_buffer::Transition<int, int>>>
      samples = buffer.sample(1);

  EXPECT_EQ(samples.size(), 1);
  EXPECT_EQ(samples[0].transition.observation, transition.observation);
  EXPECT_EQ(samples[0].transition.action, transition.action);
  EXPECT_EQ(samples[0].transition.reward, transition.reward);
  EXPECT_EQ(samples[0].transition.next_observation,
            transition.next_observation);
  EXPECT_EQ(samples[0].transition.done, transition.done);

  samples = buffer.sample(config.capacity);

  EXPECT_EQ(samples.size(), config.capacity);

  for (size_t i = 0; i < config.capacity; i++) {
    buffer.add(transition);
  }

  EXPECT_EQ(buffer.size(), config.capacity);

  std::vector<
      replay_buffer::PrioritizedSample<replay_buffer::Transition<int, int>>>
      samples_full = buffer.sample(config.capacity);

  EXPECT_EQ(samples_full.size(), config.capacity);
}

TEST(PrioritizedReplayBufferTest, ProportionalDistributionSampleTest) {
  replay_buffer::PrioritizedReplayBufferConfig config;
  config.capacity = 4;
  config.alpha = 1.0f;
  config.beta = 0.0f;
  replay_buffer::PrioritizedReplayBuffer<replay_buffer::Transition<int, int>>
      buffer(config);

  for (int i = 0; i < 4; ++i) {
    replay_buffer::Transition<int, int> transition(i, i, 1.0f, i + 1, false);
    buffer.add(transition);
  }

  buffer.update_priorities({0, 1, 2, 3}, {1.0f, 2.0f, 3.0f, 4.0f});

  const int num_samples = 10000;
  std::vector<int> counts(4, 0);

  for (int i = 0; i < num_samples; i++) {
    auto samples = buffer.sample(1);
    size_t index = samples[0].index;
    counts[index]++;
  }

  EXPECT_NEAR(counts[0], 1000, 500);
  EXPECT_NEAR(counts[1], 2000, 500);
  EXPECT_NEAR(counts[2], 3000, 500);
  EXPECT_NEAR(counts[3], 4000, 500);
}
