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
    IBGS* independentMultimodalBGS;

    /**
     * There is a toruble in IndependentMultimodalBGS method in bgslibrary.
     * When you free memory by deleting and then allocate memory by new,
     * someting goes wrong inside library method. It should not be a great
     * amount of exceed memory, so pointers will be stored in vector to ensure
     * correct work till background will not be necessary.
     */
    std::vector<IBGS*> toDelete;
public:
    RinkBackgroundSubtractor();

    /**
     * Substract moving foreground from video.
     *
     * @param img_input Current frame
     * @return Foreground image
     */
    cv::Mat process(cv::Mat);

    /**
     * Reload IndependentMultimodalBGS.
     * It can be useful after region changing.
     */
    void reload();

    /**
     * Releases memory from IBGS* usage.
     */
    ~RinkBackgroundSubtractor();
};