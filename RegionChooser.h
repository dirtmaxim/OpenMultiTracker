#include <opencv2/opencv.hpp>
#include <vector>

class RegionChooser {
private:
    static cv::Mat frame;
    static cv::Mat pointedFrame;
    static std::vector<cv::Point> vector;
public:
    RegionChooser(cv::Mat);
    void choose();
    static void onMouse(int, int, int, int, void *);
    cv::Mat getRegion();
};