/**
 * @file letter_node.h
 * @brief Header file for the LetterNode struct and related functions.
 *
 * This file contains the declaration of the LetterNode struct, which represents
 * a detected letter with its spatial and orientation properties, using a rotated rectangle.
 * It is used for graph-based word recognition.
 *
 * @author Aled Vaghela
 */

#ifndef LETTER_NODE_H
#define LETTER_NODE_H
#include <opencv2/opencv.hpp>

/**
 * @struct LetterNode
 * @brief Represents a recognized letter with its spatial and orientation properties.
 *
 * This struct stores information about a detected letter and its bounding box
 * as a rotated rectangle. It is used for graph-based word recognition.
 */
struct LetterNode {
    char letter;
    cv::RotatedRect rect;

    /**
     * @brief Default constructor for LetterNode.
     *
     * Initializes the letter to a blank character and the rectangle to a default value.
     */
    LetterNode() : letter('\0'), rect(cv::RotatedRect(cv::Point2f(0, 0), cv::Size2f(0, 0), 0)) {}

    /**
     * @brief Constructs a LetterNode from a recognized character and its bounding box.
     *
     * @param l The recognized letter character.
     * @param r The rotated rectangle representing the letter's bounding box.
     */
    LetterNode(char l, const cv::RotatedRect& r) : letter(l), rect(r) {}

    /**
     * @brief Default copy constructor.
     *
     * Allows for creating a copy of a LetterNode.
     *
     * @param other The LetterNode to copy from.
     */
    LetterNode(const LetterNode& other) = default;

    /**
     * @brief Default copy assignment operator.
     *
     * Allows for assigning one LetterNode to another.
     *
     * @param other The LetterNode to assign from.
     * @return A reference to the assigned LetterNode.
     */
    LetterNode& operator=(const LetterNode& other) = default;

    /**
     * @brief Compares one LetterNode to another and checks if all fields are equal.
     *
     * @param other Reference to another LetterNode.
     * @return true if all fields are equal, otherwise false.
     */
    bool operator==(const LetterNode& other) const {
        return letter == other.letter &&
            rect.center == other.rect.center &&
            rect.size == other.rect.size &&
            rect.angle == other.rect.angle;
    }
};

namespace std {
    /**
     * @struct hash<LetterNode>
     * @brief Specialization of std::hash for LetterNode.
     *
     * Provides a hash function for LetterNode based on its letter and rotated rectangle.
     */
    template <>
    struct hash<LetterNode> {
        std::size_t operator()(const LetterNode& node) const {
            std::size_t h1 = std::hash<char>()(node.letter);
            std::size_t h2 = std::hash<float>()(node.rect.center.x);
            std::size_t h3 = std::hash<float>()(node.rect.center.y);
            std::size_t h4 = std::hash<float>()(node.rect.size.width);
            std::size_t h5 = std::hash<float>()(node.rect.size.height);
            std::size_t h6 = std::hash<float>()(node.rect.angle);

            // Combine the hashes with XOR and bit-shifting
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5);
        }
    };
}

#endif

