//
// Created by Ars Poyezzhayev on 06.12.16.
//

#ifndef HOCKEYPLAYERSRECOGNIZER_MOTIONMODEL_H
#define HOCKEYPLAYERSRECOGNIZER_MOTIONMODEL_H

#pragma once
#include "../Utils/defines.h"
#include <opencv/cv.h>

class MotionModel {
public:
	cv::KalmanFilter* kalman;
	track_t deltatime;
	Point_t LastResult;

	MotionModel(Point_t p, track_t dt = 0.2, track_t Accel_noise_mag = 0.5);
	~MotionModel();

	Point_t GetPrediction();
	Point_t Update(Point_t p, bool DataCorrect);
};


#endif //HOCKEYPLAYERSRECOGNIZER_MOTIONMODEL_H
