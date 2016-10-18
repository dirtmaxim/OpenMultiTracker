#include "RegionChooser.h"

// Initialize static variables.
cv::Mat RegionChooser::frame;
cv::Mat RegionChooser::pointedFrame;
std::vector<cv::Point> RegionChooser::points;

RegionChooser::RegionChooser(cv::Mat img_input) {
    this->frame = img_input;
}

void RegionChooser::choose() {
    this->points.clear();
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
    while (RegionChooser::points.size() < 3) {
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
        points.push_back(cv::Point(x, y));

        // Skip first point.
        if (points.size() > 1) {
            cv::line(pointedFrame, points[points.size() - 2], points[points.size() - 1], cv::Scalar(255, 0, 0), 2);
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
    cv::Point* points = new cv::Point[this->points.size()];

    for (int i = 0; i < this->points.size(); i++) {
        points[i] = this->points[i];
    }

    const cv::Point* polygons[1];
    polygons[0] = points;
    int polygonsCount[1];
    polygonsCount[0] = (int) this->points.size();

    // It can fill any polygons whether convex or concave.
    cv::fillPoly(region, polygons, polygonsCount, 1, cv::Scalar(255, 255, 255));
    return region;
}