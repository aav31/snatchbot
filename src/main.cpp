#include <iostream>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include "TextDetector.h"
#include "TextRecognizer.h"

int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);
    cv::VideoCapture cap(0); // Open the default video camera

    if (cap.isOpened() == false)
    {
        std::cout << "Cannot open the video camera" << std::endl;
        return -1;
    }

    double dWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    std::cout << "Resolution of the video : " << dWidth << " x " << dHeight << std::endl;

    std::string window_name = "My Camera Feed";
    cv::namedWindow(window_name); //create a window called "My Camera Feed"

    TextDetector& textDetector{ TextDetector::getInstance() };
    TextRecognizer& textRecognizer{ TextRecognizer::getInstance() };

    while (true)
    {
        cv::Mat frame;
        bool bSuccess = cap.read(frame); // read a new frame from video 

        //Breaking the while loop if the frames cannot be captured
        if (bSuccess == false)
        {
            std::cout << "Video camera is disconnected" << std::endl;
            std::cin.get(); //Wait for any key press
            break;
        }

        cv::imshow(window_name, frame);
        cv::waitKey(0);

        std::vector<cv::RotatedRect> rotatedRectangles;
        textDetector.detect(frame, rotatedRectangles);

        for (const cv::RotatedRect& rotatedRectangle : rotatedRectangles)
        {
            textRecognizer.recognize(frame, rotatedRectangle);
        }
        std::cout << "One cycle of loop finished" << std::endl;

        //wait for for 10 ms until any key is pressed.  
        //If the 'Esc' key is pressed, break the while loop.
        //If any other key is pressed, continue the loop 
        //If any key is not pressed withing 10 ms, continue the loop 
        if (cv::waitKey(10) == 27)
        {
            std::cout << "Esc key is pressed by user. Stopping the video" << std::endl;
            break;
        }
    }

    return 0;

}

