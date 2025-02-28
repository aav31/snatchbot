#include <gtest/gtest.h>
#include "letter_node.h"

class LetterNodeTest : public ::testing::Test {
protected:
    LetterNode A1, A1_copy, B1, A2;

    void SetUp() override {
        cv::Point2f center1(100, 50);
        cv::Point2f center2(200, 100);
        cv::Size2f size1(10, 10);
        cv::RotatedRect rotatedRect1(center1, size1, 0.0);
        cv::RotatedRect rotatedRect2(center2, size1, 0.0);

        A1 = LetterNode('A', rotatedRect1);
        A1_copy = LetterNode('A', rotatedRect1);
        B1 = LetterNode('B', rotatedRect1);
        A2 = LetterNode('A', rotatedRect2);
    }
};

TEST_F(LetterNodeTest, Equals) {
    EXPECT_EQ(A1, A1);
    EXPECT_EQ(A1, A1_copy);
    EXPECT_NE(A1, B1);
    EXPECT_NE(A1, A2);
}

TEST_F(LetterNodeTest, Hash) {
    std::hash<LetterNode> hasher;
    EXPECT_EQ(hasher(A1), hasher(A1)) << "Identical objects should have the same hash";
    EXPECT_EQ(hasher(A1), hasher(A1_copy)) << "Identical objects should have the same hash";
    EXPECT_NE(hasher(A1), hasher(B1)) << "Different objects should have different hashes";
    EXPECT_NE(hasher(A1), hasher(A2)) << "Different objects should have different hashes";
}

TEST_F(LetterNodeTest, Assignment) {
    LetterNode assignedNode = A1;
    EXPECT_EQ(assignedNode, A1) << "Assigned node should be equal to the original node";

    assignedNode = B1;
    EXPECT_EQ(assignedNode, B1) << "Assigned node should be equal to the new assigned node";
    EXPECT_NE(assignedNode, A1) << "Assigned node should no longer equal the original node";

    assignedNode = assignedNode;
    EXPECT_EQ(assignedNode, B1) << "Self-assignment should leave the node unchanged";
}
