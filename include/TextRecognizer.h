#ifndef TEXT_RECOGNIZER_H
#define TEXT_RECOGNIZER_H
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <leptonica/allheaders.h>
#include <tesseract/ocrclass.h>
#include <tesseract/baseapi.h>


class TextRecognizer {
private:
    tesseract::TessBaseAPI tess;

    // Private constructor
    TextRecognizer(const char* dataPath = NULL, const std::string& lang = "eng") {
        if (tess.Init(dataPath, lang.c_str(), tesseract::OEM_LSTM_ONLY)) {
            throw std::runtime_error("Could not initialize tesseract.");
        }
        tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);  // Detect orientation AND recognize text

        tess.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        tess.SetVariable("user_defined_dpi", "300");

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
        // Rotate the image
        cv::Mat rotatedImage;
        cv::Mat rotationMatrix = cv::getRotationMatrix2D(rotatedRect.center, rotatedRect.angle, 1.0);
        cv::warpAffine(frame, rotatedImage, rotationMatrix, frame.size(), cv::INTER_CUBIC);
        
        // Crop the image
        cv::Mat croppedImage;
        cv::getRectSubPix(rotatedImage, rotatedRect.size, rotatedRect.center, croppedImage);

        // Resize
        cv::Mat resizedImage;
        double scale_factor = 300.0 / 70.0; // Since Tesseract assumes 70 DPI by default
        cv::resize(croppedImage, resizedImage, cv::Size(), scale_factor, scale_factor, cv::INTER_CUBIC);

        // Convert to grayscale
        cv::Mat grayImage;
        cv::cvtColor(resizedImage, grayImage, cv::COLOR_BGR2GRAY);

        // Blur
        cv::Mat blurredImage;
        cv::GaussianBlur(grayImage, blurredImage, cv::Size(5, 5), 0);

        // Threshold
        cv::Mat thresholdImage;
        cv::threshold(blurredImage, thresholdImage, 90, 255, cv::THRESH_BINARY);

        for (int i = 0; i < 4; ++i) {
            cv::rotate(thresholdImage, thresholdImage, cv::ROTATE_90_CLOCKWISE); // For rotating 90 degrees clockwise
            // Show the preprocessed image (for debugging)
            cv::namedWindow("Preprocessed Image");
            cv::imshow("Preprocessed Image", thresholdImage);
            cv::waitKey(0);
            cv::destroyWindow("Preprocessed Image");
            tess.SetImage(thresholdImage.data, thresholdImage.cols, thresholdImage.rows, 1, thresholdImage.step);
            char* text = tess.GetUTF8Text();
            int* confidences = tess.AllWordConfidences();
            std::cout << "OCR output: " << text;
            std::cout << "Confidence: " << confidences[0] << std::endl;
            delete[] text;
            delete[] confidences;
        }
    }
};


#endif
