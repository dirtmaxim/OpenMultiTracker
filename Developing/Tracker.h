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
#include "OcclusionHandler.h"
#include "ObjectState.h"

typedef float track_t;
typedef cv::Point_<track_t> Point_t;

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
    std::vector<std::unique_ptr<Track>> oldTracks;


    /**
   * Class for handling occlusions
   */
    OcclusionHandler occlusionHandler;

    //TODO: remake with variable number of arguments, including the rectangles of different detections that should be represented on the video
    void update(const std::vector<ObjectState>& detections, cv::Mat* imgOutput);

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