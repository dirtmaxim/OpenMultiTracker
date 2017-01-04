//
// Created by Ars Poyezzhayev on 05.01.17.
//

#ifndef HOCKEYPLAYERSRECOGNIZER_TRACK_H
#define HOCKEYPLAYERSRECOGNIZER_TRACK_H

#include <array>
#include "Track.h"
#include "../Utils/defines.h"
#include "MotionModel.h"

/**
 *  Representation of a track with all necessary information about object state
 */
class Track {
public:
    std::vector<Point_t> trace; //previous states of the tracked object
    std::vector<int> relatedTracks; //tracks that are connected with this track
    size_t track_id;
    size_t skipped_frames;

    Track(const Point_t& p, const cv::Rect& rect, track_t dt, track_t Accel_noise_mag, size_t trackID, std::vector<int> RelatedTracks = {0});

    /**
     * Method for calculating distance between points in the feature space. Used for distance between centroid of
     * potential plot and predicted track position.
     * @param p - point of new measure
     * @return - distance
     */
    track_t calculateDistance(const Point_t &p);

    /**
     * Overloaded method for calculating distance between centers of bounding boxes of potential plot and last
     * track position.
     * @param r - bpunding box of new measure
     * @return - distance
     */
    track_t calculateDistance(const cv::Rect &r);

    /**
     * Method that performs all basic steps of track management and model maintenace.
     * @param p
     * @param rect
     * @param dataCorrect
     * @param max_trace_length
     */
    void update(const Point_t &p, const cv::Rect &rect, bool dataCorrect, size_t max_trace_length);

    cv::Rect getLastRect();

    Point_t getPrediction();

private:
    Point_t prediction; // predicted new state for the track
    cv::Rect lastRect;
    MotionModel motionModel; // filter for motion model
};


#endif //HOCKEYPLAYERSRECOGNIZER_TRACK_H
