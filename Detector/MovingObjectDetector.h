//
// Created by Ars Poyezzhayev on 06.12.16.
//

#ifndef HOCKEYPLAYERSRECOGNIZER_MOVINGOBJECTDETECTOR_H
#define HOCKEYPLAYERSRECOGNIZER_MOVINGOBJECTDETECTOR_H

#pragma once
//#include "BackgroundSubtract.h"
#include <iostream>
#include <vector>
#include "../Utils/defines.h"

class MovingObjectDetector {
private:
    void detectContour();
//    std::unique_ptr<BackgroundSubtract> m_bs;
    std::vector<cv::Rect> m_rects;
    std::vector<Point_t> m_centers;
    cv::Mat m_fg;

    cv::Size m_minObjectSize;

public:
    const std::vector<Point_t>& detect(cv::Mat &gray);

    MovingObjectDetector(cv::Mat& gray);
    ~MovingObjectDetector(void);

    void setMinObjectSize(cv::Size minObjectSize);
    const std::vector<cv::Rect>& getDetects() const;
};

#endif //HOCKEYPLAYERSRECOGNIZER_MOVINGOBJECTDETECTOR_H
