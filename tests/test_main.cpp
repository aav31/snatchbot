// main.cpp
#include <gtest/gtest.h>

TEST(TestSuiteExample, TestExample) {
    EXPECT_EQ(1 + 1, 2) << "Quick maths";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}