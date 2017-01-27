//
// Created by Ars Poyezzhayev on 13.01.17.
//

#include "ObjectState.h"

Point_t ObjectState::coord() const {
    return (rect.tl()+rect.br())/2;
}

cv::Rect ObjectState::boundingBox() const {
    return rect;
}

ObjectState::ObjectState(cv::Rect boundingBox) {
    rect = boundingBox;
}

ObjectState::ObjectState(const Point_t &center, track_t width, track_t height) {
    rect = cv::Rect_<track_t >(center.x - width/2, center.y-height/2, width, height);
}

ObjectState::ObjectState() {
}

ObjectState::~ObjectState() {

}
