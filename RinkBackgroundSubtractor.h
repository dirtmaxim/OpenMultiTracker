#include <opencv2/opencv.hpp>
#include <opencv2/bgsegm.hpp>

/**
 * It allows you to distinguish foreground by using
 * Gaussian Mixture-based Background/Foreground Segmentation Algorithm.
 */
class RinkBackgroundSubtractor {
private:
    /**
     * Gaussian Mixture-based Background/Foreground Segmentation Algorithm.
     */
    cv::Ptr<cv::bgsegm::BackgroundSubtractorMOG> backgroundSubtractorMOG;
public:
    RinkBackgroundSubtractor();

    /**
     * Substract moving foreground from video.
     *
     * @param img_input Current frame
     * @return Foreground image
     */
    cv::Mat process(cv::Mat);
};