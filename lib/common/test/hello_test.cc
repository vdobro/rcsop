#include <gtest/gtest.h>

TEST(RcsopCommonTest, SanityCheck) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");

    // Expect equality.
    EXPECT_EQ(2 * 2, 4);
}
