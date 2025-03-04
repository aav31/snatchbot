/**
 * @file main.cpp
 * @brief Main application for real-time text detection and recognition.
 *
 * This program captures video from the default camera, detects text regions,
 * and recognizes text in those regions using OpenCV and Tesseract OCR.
 *
 * @author Aled Vaghela
 */

#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include "text_detector.h"
#include "text_recognizer.h"
#include "snatchable_word_generator.h"

/**
 * @brief Initializes the camera and text processing tools.
 *
 * @param cap Reference to the VideoCapture object.
 * @param window_name Name of the display window.
 * @throw std::runtime_error If cannot initialize.
 */
void initialize(cv::VideoCapture& cap, const std::string& window_name) {
    // initialize camera
    cap.open(0); // default video camera
    if (!cap.isOpened()) {
        throw std::runtime_error("Cannot open camera.");
    }
    double dWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    double dHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    std::cout << "Resolution of the video: " << dWidth << " x " << dHeight << std::endl;
    cv::namedWindow(window_name);

    // initialize text processing tools
    TextDetector::getInstance();
    TextRecognizer::getInstance();
    SnatchableWordGenerator::getInstance(); 
}

/**
 * @brief Processes a video frame by detecting and recognizing text.
 *
 * This function uses a singleton TextDetector to locate text regions in the frame
 * and a singleton TextRecognizer to extract words from those regions.
 *
 * @param frame Reference to the video frame to be processed.
 */
void processFrame(cv::Mat& frame) {
    TextDetector& textDetector = TextDetector::getInstance();
    TextRecognizer& textRecognizer = TextRecognizer::getInstance();
    SnatchableWordGenerator& snatchableWordGenerator = SnatchableWordGenerator::getInstance();
    std::vector<cv::RotatedRect> tileLocations = textDetector.getTileLocations(frame);
    std::vector<std::string> words = textRecognizer.generateWords(frame, tileLocations);
    std::cout << "Words:\n";
    std::for_each(words.begin(), words.end(), [](std::string x) { std::cout << x << std::endl; });
    std::vector<std::string> snatchableWords = snatchableWordGenerator.generateSnatchableWords(words);
    std::cout << "Snatchable words:\n";
    std::for_each(snatchableWords.begin(), snatchableWords.end(), [](std::string x) { std::cout << x << std::endl; });
    std::cout << "Frame processed." << std::endl;
}

/**
 * @brief Displays available button options for user interaction.
 */
void displayButtonOptions() {
    std::cout << "\n===== Available Actions =====\n";
    std::cout << "Press Enter: Perform text recognition on the current frame.\n";
    std::cout << "Press Esc: Exit the application.\n";
    std::cout << "=============================\n";
}

/**
 * @brief Main function to run the real-time text detection and recognition application.
 *
 * This function initializes the camera, captures video frames, and allows the user
 * to perform text recognition or exit using keyboard input.
 *
 * @return 0 on successful execution, -1 on failure.
 */
int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING); // reduce OpenCV log level
    cv::VideoCapture cap;
    std::string window_name = "My Camera Feed";
    try {
        initialize(cap, window_name);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    displayButtonOptions();
    while (true) {
        cv::Mat frame;
        bool bSuccess = cap.read(frame); // read a new frame from video

        if (!bSuccess) {
            std::cerr << "Video camera is disconnected" << std::endl;
            return -1;
        }

        cv::imshow(window_name, frame);
        int key = cv::waitKey(10); // wait for 10 ms until a key is pressed

        switch (key) {
        case 13: // Enter key
            processFrame(frame);
            displayButtonOptions();
            break;
        case 27: // Escape key
            std::cout << "Esc key is pressed by user. Stopping the video." << std::endl;
            return 0;
        default:
            continue;
        }
    }

    return 0;
}
