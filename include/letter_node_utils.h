/**
 * @file letter_node_utils.h
 * @brief Header file for the LetterNode related functions.
 *
 * This file contains graph based utility functions related to the 
 * LetterNode class.
 *
 * @author Aled Vaghela
 */

#ifndef LETTER_NODE_UTILS_H
#define LETTER_NODE_UTILS_H
#include <unordered_map>
#include <unordered_set>

namespace LetterNodeUtils {
    /**
     * @brief Creates a graph of LetterNodes based on adjacency.
     *
     * This function takes a vector of LetterNodes and an adjacency strategy, and
     * constructs an undirected graph represented as an unordered_map.
     *
     * @param letterNodes A vector of LetterNodes to be connected in the graph.
     * @param isAdjacent Function pointer to determine adjacency between nodes.
     * @return A graph represented as an unordered_map, where each LetterNode is mapped
     *         to its set of adjacent LetterNodes.
     */
    std::unordered_map<LetterNode, std::unordered_set<LetterNode>> createLetterNodeGraph(
        const std::vector<LetterNode>& letterNodes, bool (*isAdjacent)(LetterNode, LetterNode)) {
        std::unordered_map<LetterNode, std::unordered_set<LetterNode>> graph;
        for (const LetterNode& u : letterNodes) {
            for (const LetterNode& v : letterNodes) {
                if (isAdjacent(u, v)) {
                    graph[u].insert(v);
                    graph[v].insert(u);
                }
            }
        }
        return graph;
    }

    /**
     * @brief Computes the adjacency of two letter nodes.
     *
     * Two nodes are adjacent if their minimum area bounding box is less
     * 3 * (average tile size).
     *
     * @param u A letter node.
     * @param v Another letter node.
     * @return A bool indicating whether they are adjacent or not.
     */
    bool boundingBoxAdjacencyStrategy(LetterNode u, LetterNode v) {
        cv::Point2f pointsU[4];
        cv::Point2f pointsV[4];
        u.rect.points(pointsU); // Get the 4 corners of the first rectangle
        v.rect.points(pointsV); // Get the 4 corners of the second rectangle

        // Compute the minimum area bounding box for the combined points
        std::vector<cv::Point2f> allPoints(pointsU, pointsU + 4);
        allPoints.insert(allPoints.end(), pointsV, pointsV + 4);
        cv::RotatedRect boundingBox = cv::minAreaRect(allPoints);

        float averageTileSize = 0.5 * (u.rect.size.area() + v.rect.size.area());
        float threshold = 3 * averageTileSize;
        return boundingBox.size.area() < threshold;
    }

    /**
     * @brief Helper function to perform depth first search on a graph node.
     * 
     * This function helps find each connected component represented as a word.
     * 
     * @param u The node on which to start the depth first search.
     * @param graph The graph on which to perform the depth first search.
     * @param visited Nodes which have been visited and therefore do not want to visit again.
     * @param word Representing a connected component within the graph.
     */
    void dfs(const LetterNode u, const std::unordered_map<LetterNode, std::unordered_set<LetterNode>>& graph, std::unordered_set<LetterNode>& visited, std::string& word) {
        visited.insert(u);
        word += u.letter;
        for (const LetterNode& v : graph.at(u)) {
            if (visited.contains(v)) {
                continue;
            }
            else {
                dfs(v, graph, visited, word);
            }
        }
    }

    /**
     * @brief Converts the letter node graph into the of connected components.
     * 
     * Each connected component is a word. Each word is formed by going through
     * each node and performing a depth first search.
     * 
     * @param graph An adjacency representation of a graph.
     * @return A vector of words representing the connected components.
     */
    std::vector<std::string> findConnectedComponents(const std::unordered_map<LetterNode, std::unordered_set<LetterNode>>& graph) {
        std::unordered_set<LetterNode> visited{};
        std::vector<std::string> words{};
        for (const auto& x : graph) {
            LetterNode u = x.first;
            if (visited.contains(u)) { 
                continue;
            }
            else {
                std::string word = "";
                dfs(u, graph, visited, word);
                words.push_back(word);
            }
        }
        return words;
    }
}

#endif
