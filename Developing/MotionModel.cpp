//
// Created by Ars Poyezzhayev on 06.12.16.
//

#include "MotionModel.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

//---------------------------------------------------------------------------
//                  Motion Model based on Kalman Filter
//---------------------------------------------------------------------------
MotionModel::MotionModel(Point_t pt, track_t dt, track_t Accel_noise_mag)
{
    //time increment (lower values makes target more "massive")
    deltatime = dt; //0.2

    // We don't know acceleration, so, assume it to process noise.
    // But we can guess, the range of acceleration values which can be achieved by tracked object.
    // Process noise. (standard deviation of acceleration: m/s^2)
    // shows, how much target can accelerate.
    //track_t Accel_noise_mag = 0.5;

    //4 state variables, 2 measurements
    kalman = new cv::KalmanFilter( 4, 2, 0 );
    // Transition cv::Matrix
    kalman->transitionMatrix = (cv::Mat_<track_t>(4, 4) << 1, 0, deltatime, 0, 0, 1, 0, deltatime, 0, 0, 1, 0, 0, 0, 0, 1);

    // init...
    predictedState = pt;
    kalman->statePre.at<track_t>(0) = pt.x; // x
    kalman->statePre.at<track_t>(1) = pt.y; // y

    kalman->statePre.at<track_t>(2) = 0;
    kalman->statePre.at<track_t>(3) = 0;

    kalman->statePost.at<track_t>(0) = pt.x;
    kalman->statePost.at<track_t>(1) = pt.y;

    cv::setIdentity(kalman->measurementMatrix);

    kalman->processNoiseCov = (cv::Mat_<track_t>(4, 4) <<
                                                       pow(deltatime,4.0)/4.0	,0						,pow(deltatime,3.0)/2.0		,0,
            0						,pow(deltatime,4.0)/4.0	,0							,pow(deltatime,3.0)/2.0,
            pow(deltatime,3.0)/2.0	,0						,pow(deltatime,2.0)			,0,
            0						,pow(deltatime,3.0)/2.0	,0							,pow(deltatime,2.0));


    kalman->processNoiseCov*=Accel_noise_mag;

    setIdentity(kalman->measurementNoiseCov, cv::Scalar::all(0.1));

    setIdentity(kalman->errorCovPost, cv::Scalar::all(.1));

}

/**
 * Destructor
 */
MotionModel::~MotionModel()
{
    delete kalman;
}

/**
 * Method that estimates the new state of the object
 */
void MotionModel::predict()
{
    cv::Mat prediction = kalman->predict();
    predictedState = Point_t(prediction.at<track_t>(0), prediction.at<track_t>(1)); // predict x and y coord of position
}

/**
 * Update the model of Kalman Filter with new data.
 * @param p - state of new data
 * @param DataCorrect - if true than the filter is updated with the real data.
 * @return
 */
Point_t MotionModel::update(Point_t p, bool DataCorrect)
{
    predict();

    cv::Mat measurement(2, 1, Mat_t(1));

    if(!DataCorrect) // if not appropriate measurement
    {
        measurement.at<track_t>(0) = predictedState.x;  //update using prediction
        measurement.at<track_t>(1) = predictedState.y;
    }
    else
    {
        measurement.at<track_t>(0) = p.x;  //update using measurements
        measurement.at<track_t>(1) = p.y;
    }
    // Correction
    cv::Mat estimated = kalman->correct(measurement); // adjust the filter and estimate state of the object by weight summing of the prediction and measured state
    predictedState.x = estimated.at<track_t>(0); // adjust predicted state by using the actual measurement
    predictedState.y = estimated.at<track_t>(1);
    return predictedState;
}
//---------------------------------------------------------------------------