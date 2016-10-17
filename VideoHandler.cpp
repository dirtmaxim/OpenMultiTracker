#include "VideoHandler.h"

VideoHandler::VideoHandler(char const* file_name) {
    this->videoCapture = cv::VideoCapture(file_name);
    this->width = (int) videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
    this->height = (int) videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
    this->fps = (int) videoCapture.get(CV_CAP_PROP_FPS);
}

void VideoHandler::handle() {
    while (true) {
        cv::Mat img_input;
        bool isSuccess = this->videoCapture.read(img_input);

        if (!isSuccess) {
            break;
        }

        // Resize the frame.
        cv::resize(img_input, img_input, cv::Size(this->truncatedWidth, (int) ((float) this->truncatedWidth / this->width * this->height)));

        // Background subtraction.
        img_input = backgroundSubtractor.process(img_input);

        // Threshold.
        cv::threshold(img_input, img_input, 200, 255, cv::THRESH_BINARY);

        // First frame might be empty.
        if (img_input.data != NULL) {
            cv::imshow("HockeyPlayerRecognizer", img_input);
        }

        if (cv::waitKey(fps) >= 0) {
            break;
        }
    }
}

VideoHandler::~VideoHandler() {
    cv::destroyAllWindows();
}