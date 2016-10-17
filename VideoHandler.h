#include "RinkBackgroundSubtractor.h"

/**
 * This class composes all kind of work on video stream.
 */
class VideoHandler {
private:
    /**
     * OpenCV class to capture video.
     */
    cv::VideoCapture videoCapture;

    /**
     * Class that delegates to IndependentMultimodalBGS method from bgslibrary.
     */
    RinkBackgroundSubtractor backgroundSubtractor;

    /**
     * Region of interess. It will be loaded from file at first start
     * if file exists or user will be able to create new one by RegionChooser automatical usage.
     */
    cv::Mat region;

    /**
     * Original width of frame.
     */
    int width;

    /**
     * Original height of frame.
     */
    int height;

    /**
     * Fps of video stream.
     */
    int fps;

    /**
     * Original width will be truncated due to performance reasons.
     * Height will be truncated in proportional manner.
     */
    const int truncatedWidth = 500;

    /**
     *
     * Method delegates to RegionChooser class and add warning lables onto image.
     *
     * @param img_input Current frame of image
     * @return Code of pressed key
     */
    int chooseRegion(cv::Mat);
public:
    /**
     * It performs initialization and truncates frame according to {@link #truncatedWidth}.
     * If there is no region of interst, it will suggest to create one.
     *
     * @param file_name Name of video file with hockey game.
     */
    VideoHandler(char const*);

    /**
     * Handle the video stream using necessary classes.
     */
    void handle();

    /**
     * Releases memory from windows.
     */
    ~VideoHandler();
};