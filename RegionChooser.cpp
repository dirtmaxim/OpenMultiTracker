#include "RegionChooser.h"

// Initialize static variables.
cv::Mat RegionChooser::frame;
cv::Mat RegionChooser::pointedFrame;
std::vector<cv::Point> RegionChooser::vector;

RegionChooser::RegionChooser(cv::Mat img_input) {
    this->frame = img_input;
}

void RegionChooser::choose() {
    this->vector.clear();
    this->frame.copyTo(this->pointedFrame);

    // Put a label onto the image.
    cv::Mat promptImage;
    this->pointedFrame.copyTo(promptImage);
    cv::putText(promptImage, "Select the area of detection", cv::Point(20, 20),
                cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
    cv::imshow("HockeyPlayerDetector", promptImage);
    cv::setMouseCallback("HockeyPlayerDetector", onMouse);
    cv::waitKey(0);

    // If user selects less than 3 points, then show warning.
    while (RegionChooser::vector.size() < 3) {
        cv::Mat warningImage;
        this->pointedFrame.copyTo(warningImage);
        cv::putText(warningImage, "Select more than two points", cv::Point(20, 20),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
        cv::imshow("HockeyPlayerDetector", warningImage);
        cv::waitKey(0);
    }
}

void RegionChooser::onMouse(int event, int x, int y, int, void*) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        vector.insert(vector.end(), cv::Point(x, y));

        // Skip first point.
        if (vector.size() > 1) {
            cv::line(pointedFrame, vector[vector.size() - 2], vector[vector.size() - 1], cv::Scalar(255, 0, 0), 2);
            cv::imshow("HockeyPlayerDetector", pointedFrame);
        } else {
            cv::Mat warningImage;
            pointedFrame.copyTo(warningImage);
            cv::putText(warningImage, "Select second point", cv::Point(20, 20),
                        cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
            cv::imshow("HockeyPlayerDetector", warningImage);
        }
    }
}

cv::Mat RegionChooser::getRegion() {
    cv::Mat region(this->pointedFrame.size(), CV_8UC1, cv::Scalar(0, 0, 0));
    cv::fillConvexPoly(region, this->vector, cv::Scalar(255, 255, 255));
    return region;
}
