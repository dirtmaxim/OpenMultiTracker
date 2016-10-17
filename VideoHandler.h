#include "RinkBackgroundSubtractor.h"

class VideoHandler {
private:
    cv::VideoCapture videoCapture;
    RinkBackgroundSubtractor backgroundSubtractor;
    int width;
    int height;
    int fps;
    const int truncatedWidth = 500;
public:
    VideoHandler(char const* file_name);
    void handle();
    ~VideoHandler();
};