/**
 * @file text_recognizer.h
 * @brief Header file for the TextRecognizer class.
 *
 * This file contains the declaration of the TextRecognizer class:
 * Singleton class for generating the current words on the board.
 *
 * @author Aled Vaghela
 */

#ifndef TEXT_RECOGNIZER_H
#define TEXT_RECOGNIZER_H
#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <leptonica/allheaders.h>
#include <tesseract/ocrclass.h>
#include <tesseract/baseapi.h>
#include "letter_node.h"
#include "letter_node_utils.h"


/**
 * @class TextRecognizer
 * @brief Singleton class for generating the current words on the board.
 *
 * This class follows the Singleton pattern to ensure only one instance exists.
 * Recognizes single characters using tesseract OCR within given rotated rectangles 
 * provided from the TextDetector class. It then converts them to the current words
 * which have been played.
 */
class TextRecognizer {
public:
    /**
     * @brief Provides access to the single instance of the TextRecognizer class.
     * 
     * @return Reference to the single instance of the TextRecognizer class.
     */
    static TextRecognizer& getInstance() {
        static TextRecognizer instance;
        return instance;
    }

    TextRecognizer(const TextRecognizer&) = delete;
    TextRecognizer& operator=(const TextRecognizer&) = delete;

    /**
     * @brief Generates current words on the board from a raw frame and locations of tiles.
     * 
     * @param frame The raw frame from the video camera.
     * @param rotatedRectangles Represents the location of the tiles within the frame.
     * @param windowName Reference to the main window for OCR results display.
     * @param verbose If true adds extra debugging information.
     * @return Vector containing the words currently on the board.
     */
    std::vector<std::string> generateWords(const cv::Mat& frame, const std::vector<cv::RotatedRect>& rotatedRectangles, const std::string& windowName, bool verbose) {
        cv::Mat frameForDisplay = frame.clone();
        std::vector<LetterNode> letterNodes{};
        for (const auto& rotatedRectangle: rotatedRectangles) {
            // display rectangle
            cv::Point2f vertices[4]; // get vertices of rect for drawing
            rotatedRectangle.points(vertices);
            for (int j = 0; j < 4; j++) {
                int thickness{ 2 };
                cv::line(frameForDisplay, vertices[j], vertices[(j + 1) % 4], colourGreen, thickness);
            }

            // recognize letter
            std::optional<char> letter{ recognizeLetter(frame, rotatedRectangle, verbose) };
            if (letter) {
                LetterNode letterNode{ *letter, rotatedRectangle };
                letterNodes.push_back(letterNode);
                // add letter text to display
                std::string letterText(1, *letter);
                int fontFace = cv::FONT_HERSHEY_SIMPLEX;
                double fontScale = 0.5;
                int textThickness = 1;
                cv::Point2f textPosition = rotatedRectangle.center;
                cv::putText(frameForDisplay, letterText, textPosition, fontFace, fontScale, cv::Scalar{ 0, 0, 255 }, textThickness);
            }
        }
        std::unordered_map<LetterNode, std::unordered_set<LetterNode>> letterNodeGraph{ LetterNodeUtils::createLetterNodeGraph(letterNodes, LetterNodeUtils::boundingBoxAdjacencyStrategy) };
        std::vector<std::string> words{ LetterNodeUtils::findConnectedComponents(letterNodeGraph)};
        cv::imshow(windowName, frameForDisplay);
        if (verbose) {
            std::cout << "Words:\n";
            std::for_each(words.begin(), words.end(), [](std::string x) { std::cout << x << std::endl; });
        }
        return words;
    }

private:
    tesseract::TessBaseAPI tess;
    static constexpr int userDefinedDpi{ 300 };
    static inline const char* userDefinedDpiStr = "300";
    static constexpr double tileLengthInches{ 0.708661 };
    static inline cv::Scalar colourGreen{ 0, 255, 0 };

    /**
     * @brief Private constructor to prevent instantiation.
     * 
     * @param dataPath The path to the Tesseract data files.
     * @param lang The language for Tesseract OCR.
     */
    TextRecognizer(const char* dataPath = NULL, const std::string& lang = "eng") {
        if (tess.Init(dataPath, lang.c_str(), tesseract::OEM_LSTM_ONLY)) {
            throw std::runtime_error("Could not initialize tesseract.");
        }
        tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);  // Detect orientation AND recognize text

        tess.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        tess.SetVariable("user_defined_dpi", userDefinedDpiStr);
        tess.SetVariable("debug_file", "NUL");
    }

    /**
     * @brief Recognizes a single character within a given rotated rectangle.
     *
     * @param frame The raw frame from the video camera.
     * @param rotatedRect The rotated rectangle containing the tile.
     * @param verbose If true adds extra debugging information.
     * @return An optional single character if text can be recognized.
     */
    std::optional<char> recognizeLetter(const cv::Mat& frame, const cv::RotatedRect& rotatedRect, bool verbose) {
        constexpr int CONFIDENCE_THRESHOLD = 50; // Define a clear threshold
        cv::Mat preprocessedImage = preprocessImage(frame, rotatedRect);

        std::optional<char> bestGuess = std::nullopt;
        int bestGuessConfidence = 0;

        for (int i = 0; i < 4; ++i) {
            cv::rotate(preprocessedImage, preprocessedImage, cv::ROTATE_90_CLOCKWISE);

            // Perform OCR
            tess.SetImage(preprocessedImage.data, preprocessedImage.cols, preprocessedImage.rows, 1, preprocessedImage.step);
            char* text = tess.GetUTF8Text();
            int* confidences = tess.AllWordConfidences();

            // Ensure text has two characters - the letter and \n
            if (text != nullptr && confidences != nullptr && std::strlen(text) == 2) {
                if (confidences[0] > bestGuessConfidence) {
                    bestGuessConfidence = confidences[0];
                    bestGuess = text[0]; // Store the best recognized character
                }
            }

            if (verbose) displayTile(preprocessedImage, text, confidences ? confidences[0] : 0);

            tess.Clear();
        }

        if ((bestGuess) && (bestGuessConfidence > CONFIDENCE_THRESHOLD)) {
            if (verbose) std::cout << "Best guess: " << *bestGuess << " (Confidence: " << bestGuessConfidence << ")" << std::endl;
            return bestGuess;
        }

        return std::nullopt;
    }

    /**
     * @brief Preprocesses an image for text recognition.
     * 
     * @param frame The raw frame from the video.
     * @param rotatedRect The rotated rectangle containing the tile.
     * @return The preprocessed image ready for OCR.
     */
    cv::Mat preprocessImage(const cv::Mat& frame, const cv::RotatedRect& rotatedRect) const {
        cv::Mat preprocessedImage;
        
        // Rotate the image
        cv::Mat rotatedImage;
        cv::Mat rotationMatrix = cv::getRotationMatrix2D(rotatedRect.center, rotatedRect.angle, 1.0);
        cv::warpAffine(frame, rotatedImage, rotationMatrix, frame.size(), cv::INTER_CUBIC);

        // Crop the image
        cv::Mat croppedImage;
        cv::getRectSubPix(rotatedImage, rotatedRect.size, rotatedRect.center, croppedImage);

        // Resize
        cv::Mat resizedImage;
        const double tileLengthPixels{ rotatedRect.size.width };
        const double tileDpi{ tileLengthPixels / tileLengthInches };
        const double scaleFactor{ userDefinedDpi / tileDpi };
        cv::resize(croppedImage, resizedImage, cv::Size(), scaleFactor, scaleFactor, cv::INTER_CUBIC);

        // Convert to grayscale
        cv::Mat grayImage;
        cv::cvtColor(resizedImage, grayImage, cv::COLOR_BGR2GRAY);

        // Blur
        cv::Mat blurredImage;
        cv::GaussianBlur(grayImage, blurredImage, cv::Size(5, 5), 0);

        // Threshold
        cv::threshold(blurredImage, preprocessedImage, 150, 255, cv::THRESH_BINARY);

        return preprocessedImage;
    }

    /**
     * @brief Displays the tile image with tesseract OCR results.
     * 
     * @param preprocessedImage The preprocessed image of the tile.
     * @param text The raw output from tesseract OCR.
     * @param confidence The confidence level of the recognized text from tesseract OCR.
     * @return void.
     */
    void displayTile(const cv::Mat& preprocessedImage, const char* text, int confidence) const {
        // Convert the grayscale image back to color
        cv::Mat imageWithText;
        cv::cvtColor(preprocessedImage, imageWithText, cv::COLOR_GRAY2BGR);

        // Define the text to display
        std::string displayText = "OCR: " + std::string(text) + " | Conf: " + std::to_string(confidence);

        // Define the position and font properties
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.5;  // Smaller font size
        int thickness = 1;
        cv::Point textOrg(10, 20); // Top left corner

        // Put the text on the image
        cv::putText(imageWithText, displayText, textOrg, fontFace, fontScale, colourGreen, thickness);

        // Display the result
        cv::namedWindow("Preprocessed Image");
        cv::imshow("Preprocessed Image", imageWithText);
        cv::waitKey(0);
        cv::destroyWindow("Preprocessed Image");
    }
};



#endif
