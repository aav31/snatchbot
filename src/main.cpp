#include <opencv2/opencv.hpp>  // Include OpenCV for image processing
#include <tesseract/baseapi.h> // Include Tesseract for OCR
#include <iostream>            // Include iostream for input/output

int main() {
    // Create a simple black and white image with a single letter 'A'
    cv::Mat image = cv::Mat::zeros(cv::Size(100, 100), CV_8UC3);
    cv::putText(image, "A", cv::Point(30, 70), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(255, 255, 255), 2);

    // Convert the image to grayscale
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // Initialize Tesseract OCR engine
    tesseract::TessBaseAPI ocr;
    ocr.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

    // Set page segmentation mode to SINGLE_CHAR
    ocr.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);

    // Set the image for Tesseract to process
    ocr.SetImage(gray.data, gray.cols, gray.rows, 1, gray.step);

    // Get the recognized text
    char* text = ocr.GetUTF8Text();
    std::cout << "Recognized Text: " << text << std::endl;

    // Clean up
    delete[] text;
    ocr.End();

    return 0;
}

