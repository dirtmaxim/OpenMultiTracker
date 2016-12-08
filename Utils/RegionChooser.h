#include <opencv2/opencv.hpp>
#include <vector>

/**
 * It allows you to choose region of interest. Further, this region
 * will be applied as mask to video stream.
 */
class RegionChooser {
private:
    /**
     * Original passed frame.
     */
    static cv::Mat frame;

    /**
     * Frame with marked points.
     * It is needed to save state in order to change warning lables.
     */
    static cv::Mat pointedFrame;

    /**
     * Vector of region points.
     */
    static std::vector<cv::Point> points;
public:
    /**
    * @param img_input Image that will be used for mark region points
    */
    RegionChooser(cv::Mat);

    /**
     * Method for choosing region. It can be invoked as many times as needed.
     */
    void choose();

    /**
     * Mouse event handler.
     *
     * @param event Сoncrete event
     * @param x Coordinate of point
     * @param y Coordinate of point
     */
    static void onMouse(int, int, int, int, void *);

    /**
     * Method that build region image rely on vector of region points.
     *
     * @return Mask image
     */
    cv::Mat getRegion();
};