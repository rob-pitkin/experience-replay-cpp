#include <gtest/gtest.h>

TEST(BasicTest, AssertionsWork) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
    EXPECT_GT(5, 3);
}

TEST(BasicTest, ArithmeticWorks) {
    EXPECT_EQ(2 + 2, 4);
    EXPECT_EQ(5 * 3, 15);
    EXPECT_EQ(10 - 7, 3);
    EXPECT_EQ(20 / 4, 5);
}
