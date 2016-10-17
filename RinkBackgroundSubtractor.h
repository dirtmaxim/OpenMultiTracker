#include <opencv2/opencv.hpp>
#include "bgslibrary/package_bgs/IBGS.h"
#include "bgslibrary/package_bgs/db/IndependentMultimodalBGS.h"

/**
 * It allows you to distinguish foreground by using
 * IndependentMultimodalBGS method from bgslibrary.
 */
class RinkBackgroundSubtractor {
private:
    /**
     * Method of background subtraction from "bgslibrary".
     */
    IBGS* ibgs = new IndependentMultimodalBGS();
public:
    /**
     * Substract moving foreground from video.
     *
     * @param img_input Current frame
     * @return Foreground image
     */
    cv::Mat process(cv::Mat);

    /**
     * Releases memory from IBGS* usage.
     */
    ~RinkBackgroundSubtractor();
};