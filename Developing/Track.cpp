//
// Created by Ars Poyezzhayev on 05.01.17.
//
#include "Track.h"

    Track::Track(const Point_t& p, const cv::Rect& rect, track_t dt, track_t Accel_noise_mag, size_t trackID, std::vector<int> RelatedTracks):
            track_id(trackID),
            skipped_frames(0),
            prediction(p),
            lastRect(rect),
            motionModel(p, dt, Accel_noise_mag),
            relatedTracks(RelatedTracks)
    {
    }

    /**
     * Method for calculating distance between points in the feature space. Used for distance between centroid of
     * potential plot and predicted track position.
     * @param p - point of new measure
     * @return - distance
     */
    track_t Track::calculateDistance(const Point_t &p)
    {
        Point_t diff = prediction - p;
        return sqrtf(diff.x * diff.x + diff.y * diff.y);
    }

    /**
     * Overloaded method for calculating distance between centers of bounding boxes of potential plot and last
     * track position.
     * @param r - bpunding box of new measure
     * @return - distance
     */
    track_t Track::calculateDistance(const cv::Rect &r)
    {
        cv::Rect rect = getLastRect();

        std::array<track_t, 4> diff;
        diff[0] = prediction.x - lastRect.width / 2 - r.x;
        diff[1] = prediction.y - lastRect.height / 2 - r.y;
        diff[2] = static_cast<track_t>(lastRect.width - r.width);
        diff[3] = static_cast<track_t>(lastRect.height - r.height);

        track_t dist = 0;
        for (size_t i = 0; i < diff.size(); ++i)
        {
            dist += diff[i] * diff[i];
        }
        return sqrtf(dist);
    }

    /**
     * Method that performs all basic steps of track management and model maintenace.
     * @param p
     * @param rect
     * @param dataCorrect
     * @param max_trace_length
     */
    void Track::update(const Point_t &p, const cv::Rect &rect, bool dataCorrect, size_t max_trace_length)
    {
        // Estimate a new position of the track by using Kalman Filter
        motionModel.GetPrediction();
        prediction = motionModel.Update(p, dataCorrect);

        if (dataCorrect)
        {
            lastRect = rect;
        }

        if (trace.size() > max_trace_length)
        {
            trace.erase(trace.begin(), trace.end() - max_trace_length);
        }

        trace.push_back(prediction);
    }

    cv::Rect Track::getLastRect()
    {
        return cv::Rect(
                static_cast<int>(prediction.x - lastRect.width / 2),
                static_cast<int>(prediction.y - lastRect.height / 2),
                lastRect.width,
                lastRect.height);
    }

    Point_t Track::getPrediction(){
        return prediction;
    }
