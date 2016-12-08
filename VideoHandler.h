#include "RinkBackgroundSubtractor.h"
#include "Utils/OcclusionHandler.h"
#include "Developing/Tracker.h"
#include "Utils/RegionChooser.h"
#include <vector>

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
    RinkBackgroundSubtractor rinkBackgroundSubtractor;

    /**
     * Class for handling occlusions
     */
    OcclusionHandler occlusionHandler;


    /**
     * Region of interess. It will be loaded from file at first start
     * if file exists or user will be able to create new one by RegionChooser automatical usage.
     */
    cv::Mat region;

    /**
     * Mode of output video.
     * 0 - original, 1 - background subtraction, 2 - contoured image.
     */
    int mode;

    /**
     * Write output information
     */
    bool writeOutput = false;

    /**
     * Write directory
     */
    std::string writeDirectory;

    /**
     * Show system information of process in the left top corner.
     */
    bool showSystemInformation;

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
     * If you have intention to change this parameter, delete region.png at first.
     */
    const int truncatedWidth = 800;

    /**
     *
     * Method delegates to RegionChooser class and add warning lables onto image.
     *
     * @param img_input Current frame of image
     * @return Region of interest
     */
    cv::Mat chooseRegion(cv::Mat);
public:
    /**
     * It performs initialization and truncates frame according to {@link #truncatedWidth}.
     * If there is no region of interst, it will suggest to create one.
     *
     * @param file_name Name of video file with hockey game.
     */
    VideoHandler(char const*, int = 0, char*[] = nullptr);

    /**
     * Handle the video stream using necessary classes.
     */
    void handle();

    /**
     * Releases memory from windows.
     */
    ~VideoHandler();
};