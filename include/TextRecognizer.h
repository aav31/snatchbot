#ifndef TEXT_RECOGNIZER_H
#define TEXT_RECOGNIZER_H
#include <iostream>
#include <cmath>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <leptonica/allheaders.h>
#include <tesseract/ocrclass.h>
#include <tesseract/baseapi.h>


/**
 * @class TextRecognizer
 * @brief Singleton class for recognizing text on Bananagrams tiles.
 *
 * This class follows the Singleton pattern to ensure that only one instance
 * of the TextRecognizer exists. The Singleton pattern is used to manage global
 * state and resources efficiently.
 */
class TextRecognizer {
public:
    /**
     * @brief Provides access to the single instance of the TextRecognizer class.
     * @return Reference to the single instance of the TextRecognizer class.
     */
    static TextRecognizer& getInstance() {
        static TextRecognizer instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    TextRecognizer(const TextRecognizer&) = delete;
    TextRecognizer& operator=(const TextRecognizer&) = delete;

    /**
     * @brief Method to recognize text in a given rotated rectangle.
     * @param frame The frame from the video camera.
     * @param rotatedRect The rotated rectangle containing the tile.
     * @return void.
     */
    void recognize(const cv::Mat& frame, const cv::RotatedRect& rotatedRect) {
        cv::Mat preprocessedImage;
        preprocessImage(frame, rotatedRect, preprocessedImage);
        std::optional<std::string> bestGuess{ std::nullopt };
        int bestGuessConfidence{};

        for (int i = 0; i < 4; ++i) {
            cv::rotate(preprocessedImage, preprocessedImage, cv::ROTATE_90_CLOCKWISE); // For rotating 90 degrees clockwise
            tess.SetImage(preprocessedImage.data, preprocessedImage.cols, preprocessedImage.rows, 1, preprocessedImage.step);
            char* text = tess.GetUTF8Text();
            int* confidences = tess.AllWordConfidences();
            // Compute the most likely orientation
            if (confidences[0] > bestGuessConfidence) {
                bestGuessConfidence = confidences[0];
                bestGuess = text;
            }

            displayTile(preprocessedImage, text, confidences[0]);

            delete[] text;
            delete[] confidences;
        }

        if ((bestGuess) && (bestGuessConfidence > 50)) {
            std::cout << "Best guess: " << *bestGuess;
            std::cout << "Confidence: " << bestGuessConfidence << std::endl;
        }
    }

private:
    tesseract::TessBaseAPI tess;
    static constexpr int userDefinedDpi{ 300 };
    static constexpr double tileLengthInches{ 0.708661 };

    /**
     * @brief Private constructor to prevent instantiation.
     * @param dataPath The path to the Tesseract data files.
     * @param lang The language for Tesseract OCR.
     */
    TextRecognizer(const char* dataPath = NULL, const std::string& lang = "eng") {
        if (tess.Init(dataPath, lang.c_str(), tesseract::OEM_LSTM_ONLY)) {
            throw std::runtime_error("Could not initialize tesseract.");
        }
        tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);  // Detect orientation AND recognize text

        tess.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        const char* userDefinedDpiString{ std::to_string(userDefinedDpi).c_str() };
        tess.SetVariable("user_defined_dpi", userDefinedDpiString);
        tess.SetVariable("debug_file", "NUL");
    }

    /**
     * @brief Preprocesses an image for text recognition.
     * @param frame The raw image from the camera.
     * @param rotatedRect The rotated rectangle containing the tile.
     * @param preprocessedImage The preprocessed image ready for OCR.
     * @return void.
     */
    void preprocessImage(const cv::Mat& frame, const cv::RotatedRect& rotatedRect, cv::Mat& preprocessedImage) {
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
    }

    /**
     * @brief Displays the tile image with OCR results and confidence in the top left corner in green.
     * @param preprocessedImage The preprocessed image of the tile.
     * @param text The recognized text.
     * @param confidence The confidence level of the recognized text.
     * @return void.
     */
    void displayTile(const cv::Mat& preprocessedImage, const char* text, int confidence) {
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
        cv::putText(imageWithText, displayText, textOrg, fontFace, fontScale, cv::Scalar(0, 255, 0), thickness);

        // Display the result
        cv::namedWindow("Preprocessed Image");
        cv::imshow("Preprocessed Image", imageWithText);
        cv::waitKey(0);
        cv::destroyWindow("Preprocessed Image");
    }
};



#endif
