/**
 * @file text_detector.h
 * @brief Header file for the TextDetector class.
 *
 * This file contains the declaration of the TextDetector class, which is a
 * singleton class for detecting the location of letter tiles using OpenCV.
 *
 * @author Aled Vaghela
 */

#ifndef TEXT_DETECTOR_H
#define TEXT_DETECTOR_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

/**
 * @class TextDetector
 * @brief Singleton class for detecting the location of letter tiles using OpenCV.
 *
 * This class follows the Singleton pattern to ensure only one instance exists.
 * It processes raw video frames to detect the location of individual letter tiles
 * and return the locations as rotated rectangle objects.
 */
class TextDetector {
public:
    /**
     * @brief Provides access to the single instance of the TextDetector class.
     * 
     * @return Reference to the single instance of the TextDetector class.
     */
    static TextDetector& getInstance() {
        static TextDetector instance;
        return instance;
    }

    TextDetector(const TextDetector&) = delete;
    TextDetector& operator=(const TextDetector&) = delete;

    /**
     * @brief Detects the tile locations from a raw video frame.
     * 
     * @param frame The raw frame from the video camera.
     * @return A vector containing rotated rectangles that bound the detected tiles.
     * @param verbose If true adds extra debugging information.
     * @note Expects white tiles on a completely black background with black letters.
     */

    std::vector<cv::RotatedRect> getTileLocations(const cv::Mat& frame, bool verbose) const {
        cv::Mat processedFrame = preprocessFrame(frame);

        // Find contours - because there are white tiles on a black background
        // the contours should be squares
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(processedFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        std::vector<cv::RotatedRect> rotatedRectangles{};
        for (size_t i = 0; i < contours.size(); ++i) {
            // Get the rotated rectangle around the square shaped contours
            cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);

            // Only add rectangles with an acceptable aspect ratio
            float aspectRatio = rotatedRect.size.width / rotatedRect.size.height;
            if ((aspectRatio > aspectRatioUpperBound) || (aspectRatio < aspectRatioLowerBound)) continue;
            rotatedRectangles.push_back(rotatedRect);
        }

        if (verbose) displayDetectedTiles(processedFrame, rotatedRectangles);
        if (verbose) std::cout << "Number of tiles recognized: " << std::size(rotatedRectangles) << std::endl;
        return rotatedRectangles;
    }

private:
    static constexpr double aspectRatioLowerBound{ 0.8 };
    static constexpr double aspectRatioUpperBound{ 1.2 };
    
    /**
     * @brief Displays rotated rectangles of detected tiles on the preprocessed image.
     * 
     * @param preprocessedFrame The preprocessed image.
     * @param rotatedRectangles Rotated rectangles containing the letter tiles.
     * @return void.
     */
    void displayDetectedTiles(const cv::Mat& preprocessedFrame, const std::vector<cv::RotatedRect>& rotatedRectangles) const {
        // Convert the processed (grayscale) image back to a colour image for visualisation
        cv::Mat colourProcessedFrame;
        cv::cvtColor(preprocessedFrame, colourProcessedFrame, cv::COLOR_GRAY2BGR);

        for (const auto& rotatedRect : rotatedRectangles) {
            cv::Point2f vertices[4]; // get vertices of rect for drawing
            rotatedRect.points(vertices);
            for (int j = 0; j < 4; j++) {
                cv::Scalar colourGreen(0, 255, 0);
                int thickness{2};
                cv::line(colourProcessedFrame, vertices[j], vertices[(j + 1) % 4], colourGreen, thickness);
            }
        }

        cv::imshow("Detected Tiles", colourProcessedFrame);
        cv::waitKey(0);
        cv::destroyWindow("Detected Tiles");
    }

    /**
     * @brief Preprocesses a raw frame into an image ready for contour detection.
     * 
     * @param frame The raw frame from the video camera.
     * @return preprocessedFrame Black and white image for contour detection.
     * @note Contour detection requires white shapes on a black background
     */
    cv::Mat preprocessFrame(const cv::Mat& frame) const {
        cv::Mat preprocessedFrame;

        // Convert to gray, blur, and apply thresholding / canny edge detection
        cv::cvtColor(frame, preprocessedFrame, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(preprocessedFrame, preprocessedFrame, cv::Size(5, 5), 0);
        cv::threshold(preprocessedFrame, preprocessedFrame, 200, 255, cv::THRESH_BINARY);

        return preprocessedFrame;
    }

    /**
     * @brief Private constructor to prevent instantiation.
     *
     * The constructor is private to ensure that only the static getInstance() method
     * can create an instance of the TextDetector class.
     */
    TextDetector()
    {
	}

};

#endif
