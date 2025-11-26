#include "replay_buffer/sum_tree.h"

#include <gtest/gtest.h>

#include <random>
#include <stdexcept>

TEST(SumTreeConstructionTest, ValidCapacity) {
  replay_buffer::SumTree tree(10);
  EXPECT_EQ(tree.capacity(), 10);
  EXPECT_EQ(tree.total(), 0);
}

TEST(SumTreeConstructionTest, InvalidCapacityThrows) {
  EXPECT_THROW({ replay_buffer::SumTree tree(0); }, std::invalid_argument);
}

TEST(SumTreeSetAndGetTest, ValidIndex) {
  replay_buffer::SumTree tree(10);

  for (size_t i = 0; i < tree.capacity(); ++i) {
    tree.set(i, static_cast<float>(i));
    EXPECT_FLOAT_EQ(tree.get(i), static_cast<float>(i));
    EXPECT_FLOAT_EQ(tree.total(), static_cast<float>(i * (i + 1)) / 2.0f);
  }
}

TEST(SumTreeSetAndGetTest, InvalidIndexThrows) {
  replay_buffer::SumTree tree(10);
  EXPECT_THROW(tree.set(10, 1.0f), std::out_of_range);
  EXPECT_THROW(tree.get(10), std::out_of_range);
}

TEST(SumTreePropagationTest, SetUpdatesTotal) {
  replay_buffer::SumTree tree(4);

  tree.set(0, 1.0f);
  tree.set(1, 2.0f);
  tree.set(2, 3.0f);
  tree.set(3, 4.0f);

  EXPECT_FLOAT_EQ(tree.total(), 10.0f);

  tree.set(0, 5.0f);
  EXPECT_FLOAT_EQ(tree.total(), 14.0f);

  EXPECT_FLOAT_EQ(tree.get(0), 5.0f);
  EXPECT_FLOAT_EQ(tree.get(1), 2.0f);
}

TEST(SumTreePropagationTest, SamplingVerifiesTreeStructure) {
  replay_buffer::SumTree tree(4);
  for (int i = 0; i < 4; i++) {
    tree.set(i, static_cast<float>(i + 1));
  }

  EXPECT_EQ(tree.sample(0.0f), 0);
  EXPECT_EQ(tree.sample(0.5f), 0);
  EXPECT_EQ(tree.sample(1.0f), 1);
  EXPECT_EQ(tree.sample(2.5f), 1);
  EXPECT_EQ(tree.sample(3.0f), 2);
  EXPECT_EQ(tree.sample(6.0f), 3);
  EXPECT_EQ(tree.sample(9.9f), 3);
}

TEST(SumTreeSampleTest, SampleDistribution) {
  replay_buffer::SumTree tree(4);

  tree.set(0, 10.f);
  tree.set(1, 20.f);
  tree.set(2, 30.f);
  tree.set(3, 40.f);

  std::vector<int> counts(4, 0);

  const int num_samples = 10000;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(0.0f, tree.total());

  for (int i = 0; i < num_samples; i++) {
    const float random_value = dist(gen);
    const size_t index = tree.sample(random_value);
    counts[index]++;
  }

  EXPECT_GT(counts[0], 800);
  EXPECT_LT(counts[0], 1200);

  EXPECT_GT(counts[1], 1600);
  EXPECT_LT(counts[1], 2400);

  EXPECT_GT(counts[2], 2400);
  EXPECT_LT(counts[2], 3600);

  EXPECT_GT(counts[3], 3200);
  EXPECT_LT(counts[3], 4800);
}

TEST(SumTreeUpdateTest, UpdatePriority) {
  replay_buffer::SumTree tree(4);

  tree.set(0, 10.0f);
  tree.set(1, 20.0f);
  tree.set(2, 30.0f);
  tree.set(3, 40.0f);

  EXPECT_FLOAT_EQ(tree.total(), 100.0f);

  tree.set(0, 50.0f);
  EXPECT_FLOAT_EQ(tree.total(), 140.0f);

  EXPECT_FLOAT_EQ(tree.get(0), 50.0f);
  EXPECT_FLOAT_EQ(tree.get(1), 20.0f);

  EXPECT_EQ(tree.sample(0.0f), 0);
  EXPECT_EQ(tree.sample(49.0f), 0);
  EXPECT_EQ(tree.sample(50.0f), 1);
}

TEST(SumTreeEdgeCaseTest, SingleElementCapacity) {
  replay_buffer::SumTree tree(1);

  tree.set(0, 5.0f);

  EXPECT_FLOAT_EQ(tree.total(), 5.0f);

  EXPECT_EQ(tree.sample(0.0f), 0);
  EXPECT_EQ(tree.sample(0.5f), 0);
  EXPECT_EQ(tree.sample(4.9f), 0);
}
