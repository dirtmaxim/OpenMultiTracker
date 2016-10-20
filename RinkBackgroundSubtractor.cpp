#include "RinkBackgroundSubtractor.h"

RinkBackgroundSubtractor::RinkBackgroundSubtractor() {
    // History of MOG is increased to 1024.
    this->backgroundSubtractorMOG = cv::bgsegm::createBackgroundSubtractorMOG(1024);
}

cv::Mat RinkBackgroundSubtractor::process(cv::Mat img_input) {
    cv::Mat img_foreground;
    this->backgroundSubtractorMOG->apply(img_input, img_foreground);
    return img_foreground;
}