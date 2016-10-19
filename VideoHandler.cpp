#include "VideoHandler.h"
#include "RegionChooser.h"

cv::Mat VideoHandler::chooseRegion(cv::Mat img_input) {
    RegionChooser regionChooser(img_input);
    regionChooser.choose();
    cv::Mat region = regionChooser.getRegion();
    cv::Mat warningImage;
    region.copyTo(warningImage);
    cv::putText(warningImage, "Press any key to save region or \'r\' to choose another region or \'ESC\' to cancel",
                cv::Point(20, 20), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
    cv::imshow("HockeyPlayerDetector", warningImage);
    return region;
}

VideoHandler::VideoHandler(char const* file_name) {
    this->videoCapture = cv::VideoCapture(file_name);
    this->width = (int) videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
    this->height = (int) videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
    this->fps = (int) videoCapture.get(CV_CAP_PROP_FPS);
    this->region = cv::imread("region.png", CV_8UC1);

    // Show original image with found rectangles.
    this->mode = 0;

    if (this->region.data == NULL) {
        cv::Mat first_frame;
        bool isSuccess = this->videoCapture.read(first_frame);
        if (isSuccess) {

            // Resize the frame.
            cv::resize(first_frame, first_frame,
                       cv::Size(this->truncatedWidth, (int) ((float) this->truncatedWidth / this->width * this->height)));
            cv::Mat region;
            int code;

            do {
                region = chooseRegion(first_frame);
                code = cv::waitKey(0);
            } while (code == 82 || code == 114);

            this->region = region;
            cv::imwrite("region.png", this->region);
            this->rinkBackgroundSubtractor.reload();
        }
    }
}

void VideoHandler::handle() {
    // Set position pointer of video stream to the first frame.
    this->videoCapture.set(CV_CAP_PROP_POS_FRAMES, 0);

    while (true) {
        cv::Mat img_input;
        cv::Mat original_img_input;
        cv::Mat processed_img_input;
        cv::Mat contoured_img_input;
        cv::Mat* img_output = NULL;

        if (this->mode == 0) {
            img_output = &original_img_input;
        } else if (this->mode == 1) {
            img_output = &processed_img_input;
        } else if (this->mode == 2) {
            img_output = &contoured_img_input;
        }

        // Found contours of blobs.
        std::vector<std::vector<Point>> contours;
        bool isSuccess = this->videoCapture.read(img_input);

        if (!isSuccess) {
            break;
        }

        // Resize the frame.
        cv::resize(img_input, img_input,
                   cv::Size(this->truncatedWidth, (int) ((float) this->truncatedWidth / this->width * this->height)));

        // Save to stay intact.
        img_input.copyTo(original_img_input);

        // Apply region mask.
        img_input.copyTo(processed_img_input, this->region);

        // Apply Gaussian blur to reduce noise.
        cv::GaussianBlur(processed_img_input, processed_img_input, cv::Size(5, 5), 0);

        // Background subtraction.
        processed_img_input = rinkBackgroundSubtractor.process(processed_img_input);

        // Amount of frames to initialize background subtractor method.
        if (this->videoCapture.get(CV_CAP_PROP_POS_FRAMES) >= 145) {
            // Threshold.
            cv::threshold(processed_img_input, processed_img_input, 128, 255, cv::THRESH_OTSU);

            // Fill in holes.
            cv::dilate(processed_img_input, processed_img_input, cv::Mat(), Point(-1, -1), 2);

            processed_img_input.copyTo(contoured_img_input);

            // Find contours of blobs.
            cv::findContours(contoured_img_input, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            for(auto contour: contours) {
                if (cv::contourArea(contour) > 400) {

                    // Find bounding rect of contour.
                    cv::Rect rect = cv::boundingRect(contour);

                    // Draw rectangle on the frame.
                    cv::rectangle(*img_output, rect, cv::Scalar(255, 0, 0));
                }
            }
        }

        // First frame of "processed_img_input" might be empty.
        if (img_output->data != NULL) {
            if (this->videoCapture.get(CV_CAP_PROP_POS_FRAMES) < 145) {
                cv::putText(*img_output, "Initialization...",
                            cv::Point(20, 20), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
            }

            cv::imshow("HockeyPlayerDetector", *img_output);
        }

        // Video speed according to fps.
        int code = cv::waitKey(fps);

        // If user press 'R' or 'r', then activate region chooser.
        if (code == 82 || code == 114) {
            cv::Mat region;
            do {
                region = chooseRegion(img_input);
                code = cv::waitKey(0);
            } while (code == 82 || code == 114);

            // If code is not "ESC",
            // then save region and update method of subtraction.
            if (code != 27) {
                this->region = region;
                cv::imwrite("region.png", this->region);
                this->rinkBackgroundSubtractor.reload();
            }
        // If user press 'O' or 'o', then activate original video.
        } else if (code == 79 || code == 111) {
            this->mode = 0;
        // If user press 'B' or 'b', then activate background subtraction video.
        } else if (code == 66 || code == 98) {
            this->mode = 1;
        // If user press 'C' or 'c', then activate contoured video.
        } else if (code == 67 || code == 99) {
            if (this->videoCapture.get(CV_CAP_PROP_POS_FRAMES) >= 145) {
                this->mode = 2;
            }
        // Otherwise, terminate video.
        } else if (code >= 0) {
            break;
        }
    }
}

VideoHandler::~VideoHandler() {
    cv::destroyAllWindows();
}