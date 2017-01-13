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

ObjectState::ObjectState(Point_t center, track_t width, track_t height) {
    
}

ObjectState::ObjectState() {

}

ObjectState::~ObjectState() {

}
