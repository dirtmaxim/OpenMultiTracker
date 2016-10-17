#include "RinkBackgroundSubtractor.h"

cv::Mat RinkBackgroundSubtractor::process(cv::Mat img_input) {
    cv::Mat img_foreground;
    cv::Mat img_background;
    this->ibgs->process(img_input, img_foreground, img_background);
    return img_foreground;
}

RinkBackgroundSubtractor::~RinkBackgroundSubtractor() {
    delete this->ibgs;
}