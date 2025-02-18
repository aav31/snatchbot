#include <iostream>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include "TextDetector.h"
#include "TextRecognizer.h"

bool initializeCamera(cv::VideoCapture& cap, const std::string& window_name) {
    cap.open(0); // open the default video camera
    if (!cap.isOpened()) {
        std::cerr << "cannot open camera" << std::endl;
        return false;
    }
    double dWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH); // get the width of frames of the video
    double dHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT); // get the height of frames of the video
    std::cout << "Resolution of the video: " << dWidth << " x " << dHeight << std::endl;
    cv::namedWindow(window_name);
    return true;
}

void processFrame(cv::Mat& frame) {
    TextDetector& textDetector = TextDetector::getInstance();
    TextRecognizer& textRecognizer = TextRecognizer::getInstance();
    std::vector<cv::RotatedRect> rotatedRectangles;
    textDetector.detect(frame, rotatedRectangles);
    for (const auto& rotatedRectangle : rotatedRectangles) {
        textRecognizer.recognize(frame, rotatedRectangle);
    }
    std::cout << "Text recognition complete" << std::endl;
}

void displayButtonOptions() {
    std::cout << "\n===== Available Actions =====\n";
    std::cout << "Press Enter: Perform text recognition on the current frame\n";
    std::cout << "Press Esc: Exit the application\n";
    std::cout << "=============================\n";
}

int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING); // reduce OpenCV log level
    cv::VideoCapture cap;
    std::string window_name = "My Camera Feed";
    if (!initializeCamera(cap, window_name)) return -1;
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
