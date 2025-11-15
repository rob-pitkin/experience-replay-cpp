#include <gtest/gtest.h>
#include <replay_buffer/transition.h>

#include <string>
#include <vector>

TEST(TransitionTest, WorksWithSimpleTypes) {
  replay_buffer::Transition<int, int> t;
  t.observation = 1;
  t.action = 2;
  t.reward = 1.5f;
  t.next_observation = 3;
  t.done = false;
  t.priority = 1.0f;

  // Verify values
  EXPECT_EQ(1, t.observation);
  EXPECT_EQ(2, t.action);
  EXPECT_FLOAT_EQ(1.5f, t.reward);
  EXPECT_EQ(3, t.next_observation);
  EXPECT_FALSE(t.done);
  EXPECT_FLOAT_EQ(1.0f, t.priority);
}

TEST(TransitionTest, WorksWithVectorObservations) {
  replay_buffer::Transition<std::vector<float>, int> t = {
      {1.0f, 2.0f, 3.0f}, 2, 1.5f, {4.0f, 5.0f, 6.0f}, false, 1.0f};

  // Verify values
  EXPECT_FLOAT_EQ(1.0f, t.observation[0]);
  EXPECT_FLOAT_EQ(2.0f, t.observation[1]);
  EXPECT_FLOAT_EQ(3.0f, t.observation[2]);
  EXPECT_EQ(2, t.action);
  EXPECT_FLOAT_EQ(1.5f, t.reward);
  EXPECT_FLOAT_EQ(4.0f, t.next_observation[0]);
  EXPECT_FLOAT_EQ(5.0f, t.next_observation[1]);
  EXPECT_FLOAT_EQ(6.0f, t.next_observation[2]);
  EXPECT_FALSE(t.done);
  EXPECT_FLOAT_EQ(1.0f, t.priority);
}
