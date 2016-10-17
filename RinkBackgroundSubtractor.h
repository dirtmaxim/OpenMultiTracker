#include <opencv2/opencv.hpp>
#include "bgslibrary/package_bgs/IBGS.h"
#include "bgslibrary/package_bgs/db/IndependentMultimodalBGS.h"

class RinkBackgroundSubtractor {
private:
    IBGS* ibgs = new IndependentMultimodalBGS();
public:
    cv::Mat process(cv::Mat);
    ~RinkBackgroundSubtractor();
};