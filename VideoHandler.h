#include "RinkBackgroundSubtractor.h"

class VideoHandler {
private:
    cv::VideoCapture videoCapture;
    RinkBackgroundSubtractor backgroundSubtractor;
    cv::Mat region;
    int width;
    int height;
    int fps;
    const int truncatedWidth = 500;
    int chooseRegion(cv::Mat);
public:
    VideoHandler(char const*);
    void handle();
    ~VideoHandler();
};