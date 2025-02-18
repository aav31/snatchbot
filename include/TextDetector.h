#ifndef TEXT_DETECTOR_H
#define TEXT_DETECTOR_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

class TextDetector {
private:
    static constexpr double aspectRatioLowerBound{ 0.5 };
    static constexpr double aspectRatioUpperBound{ 1.5 };

    // the below values are good for a camera which is 430mm above the table
    static constexpr double areaLowerBound{ 100 }; // in pixels
    static constexpr double areaUpperBound{ 225 }; // in pixels
    static constexpr int fixedSquareSideLength{ 16 }; // in pixels
    TextDetector()
    {
	}
public:
    static TextDetector& getInstance() {
        static TextDetector instance;
        return instance;
    }

    TextDetector(const TextDetector&) = delete;
    TextDetector& operator=(const TextDetector&) = delete;

    void detect(const cv::Mat& frame, std::vector<cv::RotatedRect>& rotatedRectangles) const {
        cv::Mat processedFrame;

        // Convert to gray
        cv::cvtColor(frame, processedFrame, cv::COLOR_BGR2GRAY);

        // Blur the image to reduce noise
        cv::GaussianBlur(processedFrame, processedFrame, cv::Size(5, 5), 0);
        
        // Apply thresholding OR edge detection
        //cv::threshold(processedFrame, processedFrame, 90, 255, cv::THRESH_BINARY_INV);
        cv::Canny(processedFrame, processedFrame, 70, 200);

        // Find contours
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(processedFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        // Convert the processed (grayscale) image back to a colour image for visualisation
        cv::Mat colourProcessedFrame;
        cv::cvtColor(processedFrame, colourProcessedFrame, cv::COLOR_GRAY2BGR);
        
        // Draw the contours on the colour image (in blue)
        //cv::drawContours(colourProcessedFrame, contours, -1, cv::Scalar(255, 0, 0));

        for (size_t i = 0; i < contours.size(); ++i) {
            // Get the rotated rectangle
            cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);

            float aspectRatio = rotatedRect.size.width / rotatedRect.size.height;
            float area = rotatedRect.size.width * rotatedRect.size.height;

            // if it doesn't smell like a scrabble tile skip it
            if ((aspectRatio > aspectRatioUpperBound) 
                || (aspectRatio < aspectRatioLowerBound) 
                || (area < areaLowerBound) 
                || (area > areaUpperBound)) continue;
            
            // make sure fixed boxes are drawn around tiles
            cv::Size fixedSize(fixedSquareSideLength, fixedSquareSideLength);
            rotatedRect.size = fixedSize;

            // add the rectangle
            rotatedRectangles.push_back(rotatedRect);

            // Get the vertices of the rotated rectangle for drawing
            cv::Point2f vertices[4];
            rotatedRect.points(vertices);
            for (int j = 0; j < 4; j++) {
                cv::line(colourProcessedFrame, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0), 2); // Green lines
            }


            // Add text
            std::stringstream areaText;
            areaText << "Area: " << std::fixed << std::setprecision(2) << area;
            cv::Point2f textPosition(rotatedRect.center.x, rotatedRect.center.y);
            cv::putText(colourProcessedFrame, areaText.str(), textPosition, cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 0), 1);
        }
        // Display the result
        cv::imshow("Detected Tiles", colourProcessedFrame);
        cv::waitKey(0);
        cv::destroyWindow("Detected Tiles");
        std::cout << "Number of tiles recognized: " << std::size(rotatedRectangles) << std::endl;
    }
};

#endif
