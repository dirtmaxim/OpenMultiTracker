//
// Created by Ars Poyezzhayev on 05.01.17.
//
#include "Track.h"

    Track::Track(const ObjectState& p, track_t dt, track_t Accel_noise_mag, size_t trackID, std::vector<int> RelatedTracks):
            track_id(trackID),
            skipped_frames(0),
            prediction(p.coord()),
            lastRect(p.boundingBox()),
            motionModel(p.coord(), dt, Accel_noise_mag),
            relatedTracks(RelatedTracks)
    {
    }

    /**
     * Method that performs all basic steps of track management and model maintenace.
     * @param p
     * @param rect
     * @param dataCorrect
     * @param max_trace_length
     */
    void Track::update(const ObjectState &p, bool dataCorrect, size_t max_trace_length)
    {
        // Estimate a new position of the track by using Kalman Filter
        motionModel.GetPrediction();
        prediction = motionModel.Update(p.coord(), dataCorrect);

        if (dataCorrect)
        {
            lastRect = p.boundingBox();
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

track_t Track::calculateDistance(const ObjectState &p) {
    Point_t diff = prediction - p.coord();
    return sqrtf(diff.x * diff.x + diff.y * diff.y);
}
