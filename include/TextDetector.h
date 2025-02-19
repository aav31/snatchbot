#ifndef TEXT_DETECTOR_H
#define TEXT_DETECTOR_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

/**
 * @class TextDetector
 * @brief Singleton class for detecting and displaying Bananagrams tiles.
 *
 * This class follows the Singleton pattern to ensure that only one instance
 * of the TextDetector exists. The Singleton pattern is used to manage global
 * state and resources efficiently.
 */
class TextDetector {
public:
    /**
     * @brief Provides access to the single instance of the TextDetector class.
     * @return Reference to the single instance of the TextDetector class.
     */
    static TextDetector& getInstance() {
        static TextDetector instance;
        return instance;
    }

    TextDetector(const TextDetector&) = delete;
    TextDetector& operator=(const TextDetector&) = delete;

    /**
     * @brief Function which detects the tiles from a video frame.
     * @param frame The frame from the video camera.
     * @param rotatedRectangles Rotated rectangles containing the scrabble tiles.
     * @return void.
     *
     * The function takes in an empty rotatedRectangles vector and appends to it
     * rotated rectangles corresponding to the tiles. It expects white tiles
     * on a completely black background with black letters.
     */
    void detect(const cv::Mat& frame, std::vector<cv::RotatedRect>& rotatedRectangles) const {
        cv::Mat processedFrame;
        preprocessImage(frame, processedFrame);

        // Find contours - because there are white tiles on a black background
        // the contours should be squares
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(processedFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < contours.size(); ++i) {
            // Get the rotated rectangle around the square shaped contours
            cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);

            // Only add rectangles with an acceptable aspect ratio
            float aspectRatio = rotatedRect.size.width / rotatedRect.size.height;
            if ((aspectRatio > aspectRatioUpperBound) || (aspectRatio < aspectRatioLowerBound)) continue;
            rotatedRectangles.push_back(rotatedRect);
        }

        displayDetectedTiles(processedFrame, rotatedRectangles);
        std::cout << "Number of tiles recognized: " << std::size(rotatedRectangles) << std::endl;
    }

private:
    static constexpr double aspectRatioLowerBound{ 0.8 };
    static constexpr double aspectRatioUpperBound{ 1.2 };
    
    /**
     * @brief Displays rotated rectangles of detected tiles on the preprocessed image.
     * @param processedFrame The preprocessed image.
     * @param rotatedRectangles Rotated rectangles containing the scrabble tiles.
     * @return void.
     */
    void displayDetectedTiles(const cv::Mat& processedFrame, const std::vector<cv::RotatedRect>& rotatedRectangles) const {
        // Convert the processed (grayscale) image back to a colour image for visualisation
        cv::Mat colourProcessedFrame;
        cv::cvtColor(processedFrame, colourProcessedFrame, cv::COLOR_GRAY2BGR);

        for (const auto& rotatedRect : rotatedRectangles) {
            cv::Point2f vertices[4]; // get vertices of rect for drawing
            rotatedRect.points(vertices);
            for (int j = 0; j < 4; j++) {
                cv::line(colourProcessedFrame, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0), 2); // Green lines
            }
        }

        // Display the result
        cv::imshow("Detected Tiles", colourProcessedFrame);
        cv::waitKey(0);
        cv::destroyWindow("Detected Tiles");
    }

    /**
     * @brief Preprocesses a raw image into an image ready for contour detection.
     * @param frame The raw image from the camera.
     * @param processedFrame The preprocessed black and white image for contour detection.
     * @return void.
     * 
     * We need to convert the frame to white shapes on a black background before we do
     * contour detection.
     */
    void preprocessImage(const cv::Mat& frame, cv::Mat& processedFrame) const {
        // Convert to gray
        cv::cvtColor(frame, processedFrame, cv::COLOR_BGR2GRAY);

        // Blur the image to reduce noise
        cv::GaussianBlur(processedFrame, processedFrame, cv::Size(5, 5), 0);

        // Apply thresholding OR canny edge detection
        cv::threshold(processedFrame, processedFrame, 200, 255, cv::THRESH_BINARY);
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
