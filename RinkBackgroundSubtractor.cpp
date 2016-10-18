#include "RinkBackgroundSubtractor.h"

RinkBackgroundSubtractor::RinkBackgroundSubtractor() {
    this->independentMultimodalBGS = new IndependentMultimodalBGS();
}

cv::Mat RinkBackgroundSubtractor::process(cv::Mat img_input) {
    cv::Mat img_foreground;
    cv::Mat img_background;
    this->independentMultimodalBGS->process(img_input, img_foreground, img_background);
    return img_foreground;
}

void RinkBackgroundSubtractor::reload() {
    this->toDelete.push_back(this->independentMultimodalBGS);
    this->independentMultimodalBGS = new IndependentMultimodalBGS();
}


RinkBackgroundSubtractor::~RinkBackgroundSubtractor() {
    for (int i = 0; i < this->toDelete.size(); i++) {
        delete toDelete[i];
    }
}