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
}
