#ifndef TEXT_RECOGNIZER_H
#define TEXT_RECOGNIZER_H
#include <iostream>
#include <cmath>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <leptonica/allheaders.h>
#include <tesseract/ocrclass.h>
#include <tesseract/baseapi.h>


class TextRecognizer {
private:
    tesseract::TessBaseAPI tess;
    static constexpr int userDefinedDpi{ 300 };
    static constexpr double tileLengthInches{ 0.708661 };

    // Private constructor
    TextRecognizer(const char* dataPath = NULL, const std::string& lang = "eng") {
        if (tess.Init(dataPath, lang.c_str(), tesseract::OEM_LSTM_ONLY)) {
            throw std::runtime_error("Could not initialize tesseract.");
        }
        tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);  // Detect orientation AND recognize text

        tess.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        const char* userDefinedDpiString{ std::to_string(userDefinedDpi).c_str() };
        tess.SetVariable("user_defined_dpi", userDefinedDpiString );
        tess.SetVariable("debug_file", "NUL");

    }

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

public:
    // Singleton instance
    static TextRecognizer& getInstance() {
        static TextRecognizer instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    TextRecognizer(const TextRecognizer&) = delete;
    TextRecognizer& operator=(const TextRecognizer&) = delete;

    // Method to recognize text in a given rotated rectangle
    void recognize(const cv::Mat& frame, const cv::RotatedRect& rotatedRect) {
        cv::Mat preprocessedImage;
        preprocessImage(frame, rotatedRect, preprocessedImage);
        std::optional<std::string> bestGuess{std::nullopt};
        int bestGuessConfidence{};
        for (int i = 0; i < 4; ++i) {
            cv::rotate(preprocessedImage, preprocessedImage, cv::ROTATE_90_CLOCKWISE); // For rotating 90 degrees clockwise
            tess.SetImage(preprocessedImage.data, preprocessedImage.cols, preprocessedImage.rows, 1, preprocessedImage.step);
            char* text = tess.GetUTF8Text();
            int* confidences = tess.AllWordConfidences();
            // compute most likely orientation
            if (confidences[0] > bestGuessConfidence) {
                bestGuessConfidence = confidences[0];
                bestGuess = text;
            }
            //cv::namedWindow("Preprocessed Image");
            //cv::imshow("Preprocessed Image", preprocessedImage);
            //cv::waitKey(0);
            //cv::destroyWindow("Preprocessed Image");
            //std::cout << "OCR output: " << text;
            //std::cout << "Confidence: " << confidences[0] << std::endl;
            delete[] text;
            delete[] confidences;
        }
        if ((bestGuess) && (bestGuessConfidence > 50)) {
            std::cout << "Best guess: " << *bestGuess;
            std::cout << "Confidence: " << bestGuessConfidence << std::endl;
        }
        
    }
};


#endif
