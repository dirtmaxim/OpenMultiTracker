#include "VideoHandler.h"
#include "RegionChooser.h"

int VideoHandler::chooseRegion(cv::Mat img_input) {
    RegionChooser regionChooser(img_input);
    regionChooser.choose();
    this->region = regionChooser.getRegion();
    cv::Mat warningImage;
    this->region.copyTo(warningImage);
    cv::putText(warningImage, "Press any key to save region or \'r\' to choose another region",
                cv::Point(20, 20), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
    cv::imshow("HockeyPlayerDetector", warningImage);
    return cv::waitKey(0);
}

VideoHandler::VideoHandler(char const* file_name) {
    this->videoCapture = cv::VideoCapture(file_name);
    this->width = (int) videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
    this->height = (int) videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
    this->fps = (int) videoCapture.get(CV_CAP_PROP_FPS);
    this->region = cv::imread("region.png", CV_8UC1);

    if (this->region.data == NULL) {
        cv::Mat first_frame;
        bool isSuccess = this->videoCapture.read(first_frame);
        if (isSuccess) {

            // Resize the frame.
            cv::resize(first_frame, first_frame,
                       cv::Size(this->truncatedWidth, (int) ((float) this->truncatedWidth / this->width * this->height)));
            int code;

            do {
                code = chooseRegion(first_frame);
            } while (code == 82 || code == 114);

            cv::imwrite("region.png", this->region);
        }
    }
}

void VideoHandler::handle() {

    // Set position pointer of video stream to the first frame.
    this->videoCapture.set(CV_CAP_PROP_POS_AVI_RATIO, 0);

    while (true) {
        cv::Mat img_input;
        cv::Mat processed_img_input;
        bool isSuccess = this->videoCapture.read(img_input);

        if (!isSuccess) {
            break;
        }

        // Resize the frame.
        cv::resize(img_input, img_input,
                   cv::Size(this->truncatedWidth, (int) ((float) this->truncatedWidth / this->width * this->height)));

        // Apply region mask.
        img_input.copyTo(processed_img_input, this->region);

        // Background subtraction.
        processed_img_input = backgroundSubtractor.process(processed_img_input);

        // Threshold.
        cv::threshold(processed_img_input, processed_img_input, 128, 255, cv::THRESH_OTSU);

        // First frame might be empty.
        if (processed_img_input.data != NULL) {
            cv::imshow("HockeyPlayerDetector", processed_img_input);
        }

        int code = cv::waitKey(fps);

        // If user press 'R' or 'r', then activate region chooser.
        if (code == 82 || code == 114) {
            do {
                code = chooseRegion(img_input);
            } while (code == 82 || code == 114);

            cv::imwrite("region.png", this->region);
        } else if (code >= 0) {
            break;
        }
    }
}

VideoHandler::~VideoHandler() {
    cv::destroyAllWindows();
}