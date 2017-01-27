//
// Created by Ars Poyezzhayev on 06.01.17.
//

#ifndef HOCKEYPLAYERSRECOGNIZER_OBJECTSTATE_H
#define HOCKEYPLAYERSRECOGNIZER_OBJECTSTATE_H


#include <opencv2/core/types.hpp>
#include "../Utils/defines.h"


class ObjectState {
private:
    cv::Rect rect;

public:
    Point_t coord()const;
    cv::Rect boundingBox()const;

    ObjectState(cv::Rect boundingBox);
    ObjectState(const Point_t &center, track_t width = 0, track_t height = 0);
    ObjectState();
    ~ObjectState();
};


#endif //HOCKEYPLAYERSRECOGNIZER_OBJECTSTATE_H
