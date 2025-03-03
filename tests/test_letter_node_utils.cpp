#include <gtest/gtest.h>
#include <algorithm>
#include "letter_node.h"
#include "letter_node_utils.h"

TEST(LetterNodeUtilsTest, CreateGraphEmpty) {
    // Define a relation u ~ v always
    auto isAdjacentFunction = [](LetterNode u, LetterNode v) {return true; };
    std::vector<LetterNode> letterNodes;
    std::unordered_map<LetterNode, std::unordered_set<LetterNode>> graph{ LetterNodeUtils::createLetterNodeGraph(letterNodes, isAdjacentFunction) };
    EXPECT_EQ(std::size(graph), 0);
}

TEST(LetterNodeUtilsTest, CreateGraphNonEmpty) {
    // Define a relation u ~ v iff they have the same rectangle center
    auto isAdjacentFunction = [](LetterNode u, LetterNode v) {return u.rect.center == v.rect.center; };

    // Setup three nodes where A ~ B but A !~ C
    cv::Size2f size0(0, 0);
    cv::RotatedRect rectA(cv::Point2f(0, 0), size0, 0.0);
    LetterNode a('A', rectA);
    cv::RotatedRect rectB(cv::Point2f(0, 0), size0, 0.0);
    LetterNode b('B', rectB);
    cv::RotatedRect rectC(cv::Point2f(1, 1), size0, 0.0);
    LetterNode c('C', rectC);

    std::vector<LetterNode> letterNodes;
    letterNodes.push_back(a);
    letterNodes.push_back(b);
    letterNodes.push_back(c);
    std::unordered_map<LetterNode, std::unordered_set<LetterNode>> graph{ LetterNodeUtils::createLetterNodeGraph(letterNodes, isAdjacentFunction) };
    EXPECT_EQ(std::size(graph), 3);
    EXPECT_EQ(std::size(graph[a]), 2) << "a is related to itself and b";
    EXPECT_TRUE(graph[a].contains(a)) << "a is related to itself and b";
    EXPECT_TRUE(graph[a].contains(b)) << "a is related to itself and b";
    EXPECT_EQ(std::size(graph[b]), 2) << "b is related to itself and a";
    EXPECT_TRUE(graph[b].contains(b)) << "b is related to itself and a";
    EXPECT_TRUE(graph[b].contains(a)) << "b is related to itself and a";
    EXPECT_EQ(std::size(graph[c]), 1) << "c is only related to itself";
    EXPECT_TRUE(graph[c].contains(c)) << "c is only related to itself";
}

TEST(LetterNodeUtilsTest, BoundingBoxAdjacencyStrategy) {
    cv::Size2f size(10, 10);
    
    cv::Point2f centerA(5, 5);
    cv::RotatedRect rectA(centerA, size, 0.0);
    LetterNode A('A', rectA);

    cv::Point2f centerB(15, 5);
    cv::RotatedRect rectB(centerB, size, 0.0);
    LetterNode B('B', rectB);

    cv::Point2f centerC(5, 15);
    cv::RotatedRect rectC(centerC, size, 0.0);
    LetterNode C('C', rectC);

    EXPECT_TRUE(LetterNodeUtils::boundingBoxAdjacencyStrategy(A, B)) << "Tiles are horizonally right next to each other";
    EXPECT_TRUE(LetterNodeUtils::boundingBoxAdjacencyStrategy(A, C)) << "Tiles are vertically right next to each other";
    EXPECT_FALSE(LetterNodeUtils::boundingBoxAdjacencyStrategy(C, B)) << "Tiles are diagonal from each other";
}

class LetterNodeUtilsFindConnectedComponentsTest : public ::testing::Test {
protected:
    LetterNode a;
    LetterNode b;
    LetterNode c;
    LetterNode d;
    LetterNode e;
    LetterNode f;

    void SetUp() override {
        cv::RotatedRect rect(cv::Point2f(0, 0), cv::Size2f(0, 0), 0.0);
        a = LetterNode('A', rect);
        b = LetterNode('B', rect);
        c = LetterNode('C', rect);
        d = LetterNode('D', rect);
        e = LetterNode('E', rect);
        f = LetterNode('F', rect);
    }
};

TEST_F(LetterNodeUtilsFindConnectedComponentsTest, EmptyGraph) {
    std::unordered_map<LetterNode, std::unordered_set<LetterNode>> graph;
    std::vector<std::string> words = LetterNodeUtils::findConnectedComponents(graph);
    EXPECT_EQ(std::size(words), 0);
}

TEST_F(LetterNodeUtilsFindConnectedComponentsTest, Disconnected) {
    std::unordered_map<LetterNode, std::unordered_set<LetterNode>> graph;
    graph[a] = { a };
    graph[b] = { b };
    graph[c] = { c };
    std::vector<std::string> words = LetterNodeUtils::findConnectedComponents(graph);
    EXPECT_EQ(std::size(words), 3);
    EXPECT_TRUE(std::find(words.begin(), words.end(), "A") != words.end()) << "'A' is missing";
    EXPECT_TRUE(std::find(words.begin(), words.end(), "B") != words.end()) << "'B' is missing";
    EXPECT_TRUE(std::find(words.begin(), words.end(), "C") != words.end()) << "'C' is missing";
}

TEST_F(LetterNodeUtilsFindConnectedComponentsTest, FullyConnected) {
    std::unordered_map<LetterNode, std::unordered_set<LetterNode>> graph;
    graph[a] = { a, b, c };
    graph[b] = { a, b, c };
    graph[c] = { a, b, c };
    std::vector<std::string> words = LetterNodeUtils::findConnectedComponents(graph);
    EXPECT_EQ(std::size(words), 1);
    EXPECT_TRUE(std::find(words.begin(), words.end(), "ABC") != words.end()) << "'ABC' is missing";
}

TEST_F(LetterNodeUtilsFindConnectedComponentsTest, Complex) {
    std::unordered_map<LetterNode, std::unordered_set<LetterNode>> graph;
    graph[a] = { a, b, c };
    graph[b] = { b, a };
    graph[c] = { c, b };
    graph[d] = { d };
    graph[e] = { e, f };
    graph[f] = { f, e };
    std::vector<std::string> words = LetterNodeUtils::findConnectedComponents(graph);
    EXPECT_EQ(std::size(words), 3);
    EXPECT_TRUE(std::find(words.begin(), words.end(), "ABC") != words.end()) << "'ABC' is missing";
    EXPECT_TRUE(std::find(words.begin(), words.end(), "D") != words.end()) << "'D' is missing";
    EXPECT_TRUE(std::find(words.begin(), words.end(), "EF") != words.end()) << "'EF' is missing";
}