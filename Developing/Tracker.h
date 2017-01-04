//
// Created by Ars Poyezzhayev on 06.12.16.
//

#ifndef HOCKEYPLAYERSRECOGNIZER_TRACKER_H
#define HOCKEYPLAYERSRECOGNIZER_TRACKER_H

#pragma once
#include "MotionModel.h"
#include "DataAssociationAlgorithm.h"

#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include "../Utils/defines.h"
#include "Track.h"
#include "../Utils/OcclusionHandler.h"

typedef float track_t;
typedef cv::Point_<track_t> Point_t;

///**
// *  Representation of a track with all necessary information about object state
// */
//class Track {
//public:
//    std::vector<Point_t> trace; //previous states of the tracked object
//    std::vector<int> relatedTracks; //tracks that are connected with this track
//    size_t track_id;
//    size_t skipped_frames;
//
//    Track(const Point_t& p, const cv::Rect& rect, track_t dt, track_t Accel_noise_mag, size_t trackID, std::vector<int> RelatedTracks = {0}):
//            track_id(trackID),
//            skipped_frames(0),
//            prediction(p),
//            lastRect(rect),
//            motionModel(p, dt, Accel_noise_mag),
//            relatedTracks(RelatedTracks)
//    {
//    }
//
//    /**
//     * Method for calculating distance between points in the feature space. Used for distance between centroid of
//     * potential plot and predicted track position.
//     * @param p - point of new measure
//     * @return - distance
//     */
//    track_t calculateDistance(const Point_t &p)
//    {
//        Point_t diff = prediction - p;
//        return sqrtf(diff.x * diff.x + diff.y * diff.y);
//    }
//
//    /**
//     * Overloaded method for calculating distance between centers of bounding boxes of potential plot and last
//     * track position.
//     * @param r - bpunding box of new measure
//     * @return - distance
//     */
//    track_t calculateDistance(const cv::Rect &r)
//    {
//        cv::Rect rect = getLastRect();
//
//        std::array<track_t, 4> diff;
//        diff[0] = prediction.x - lastRect.width / 2 - r.x;
//        diff[1] = prediction.y - lastRect.height / 2 - r.y;
//        diff[2] = static_cast<track_t>(lastRect.width - r.width);
//        diff[3] = static_cast<track_t>(lastRect.height - r.height);
//
//        track_t dist = 0;
//        for (size_t i = 0; i < diff.size(); ++i)
//        {
//            dist += diff[i] * diff[i];
//        }
//        return sqrtf(dist);
//    }
//
//    /**
//     * Method that performs all basic steps of track management and model maintenace.
//     * @param p
//     * @param rect
//     * @param dataCorrect
//     * @param max_trace_length
//     */
//    void update(const Point_t &p, const cv::Rect &rect, bool dataCorrect, size_t max_trace_length)
//    {
//        // Estimate a new position of the track by using Kalman Filter
//        motionModel.GetPrediction();
//        prediction = motionModel.Update(p, dataCorrect);
//
//        if (dataCorrect)
//        {
//            lastRect = rect;
//        }
//
//        if (trace.size() > max_trace_length)
//        {
//            trace.erase(trace.begin(), trace.end() - max_trace_length);
//        }
//
//        trace.push_back(prediction);
//    }
//
//    cv::Rect getLastRect()
//    {
//        return cv::Rect(
//                static_cast<int>(prediction.x - lastRect.width / 2),
//                static_cast<int>(prediction.y - lastRect.height / 2),
//                lastRect.width,
//                lastRect.height);
//    }
//
//    Point_t getPrediction(){
//        return prediction;
//    }
//
//private:
//    Point_t prediction; // predicted new state for the track
//    cv::Rect lastRect;
//    MotionModel motionModel; // filter for motion model
//};


//=========================================Tracker=====================================

class Tracker{
public:
    /**
     * Constructor
     * @param dt_
     * @param Accel_noise_mag_
     * @param dist_thres_
     * @param maximum_allowed_skipped_frames_
     * @param max_trace_length_
     * @return
     */
    Tracker(track_t dt_, track_t Accel_noise_mag_, track_t dist_thres_ = 60, size_t maximum_allowed_skipped_frames_ = 10, size_t max_trace_length_ = 10);

    Tracker();

    ~Tracker(void);

    /**
     * Enum for selection of the type of distance calculation
     */
    enum DistType
    {
        CentersDist = 0,
        RectsDist = 1
    };

    std::vector<std::unique_ptr<Track>> tracks;
    /**
   * Class for handling occlusions
   */
    OcclusionHandler occlusionHandler;

    //TODO: remake with variable number of arguments, including the rectangles of different detections that should be represented on the video
    void update(const std::vector<Point_t>& detections, const std::vector<cv::Rect>& rects, DistType distType, cv::Mat* imgOutput);

private:
    // The update frequency of the filter
    track_t dt;

    track_t Accel_noise_mag;

    // Distance threshold. If points are farther then this threshold then they aren't considered
    // in the data association.
    track_t distanceThreshold;
    // Number of skipped frames track is maintained until it is removed
    size_t maximum_allowed_skipped_frames;
    // Maximal length of the track
    size_t max_trace_length;

    size_t NextTrackID;


};

#endif //HOCKEYPLAYERSRECOGNIZER_TRACKER_H