//
// Created by Ars Poyezzhayev on 05.01.17.
//
#include "Track.h"

// ======================================Constructors======================================
    Track::Track(const ObjectState& p, track_t dt, track_t Accel_noise_mag, size_t trackID, std::vector<int> &RelatedTracks):
            track_id(trackID),
            skipped_frames(0),
            prediction(p.coord()),
            lastRect(p.boundingBox()),
            motionModel(p.coord(), dt, Accel_noise_mag),
            relatedTracks(RelatedTracks)
    {}

    Track::Track(const ObjectState &p, track_t dt, track_t Accel_noise_mag, size_t trackID, int RelatedTrack) :
            track_id(trackID),
            skipped_frames(0),
            prediction(p.coord()),
            lastRect(p.boundingBox()),
            motionModel(p.coord(), dt, Accel_noise_mag)
    {
        relatedTracks.push_back(RelatedTrack);
    }

    Track::Track(const ObjectState &p, track_t dt, track_t Accel_noise_mag, size_t trackID) :
            track_id(trackID),
            skipped_frames(0),
            prediction(p.coord()),
            lastRect(p.boundingBox()),
            motionModel(p.coord(), dt, Accel_noise_mag)
    {}


// =====================================/Constructors======================================



    void Track::update(const ObjectState &p, bool dataCorrect, size_t max_trace_length)
    {
        // Estimate a new position of the track by using Kalman Filter
        prediction = motionModel.update(p.coord(), dataCorrect);

        if (dataCorrect)
        {
            lastRect = p.boundingBox();
        }

        if (trace.size() > max_trace_length)
        {
            trace.erase(trace.begin(), trace.end() - max_trace_length);
        }

        trace.push_back(ObjectState(prediction,lastRect.width, lastRect.height));
    }

    cv::Rect Track::getLastPredictedRect()const {
        return cv::Rect(
                static_cast<int>(prediction.x - lastRect.width / 2),
                static_cast<int>(prediction.y - lastRect.height / 2),
                lastRect.width,
                lastRect.height);
    }

    cv::Rect Track::getLastRect(){
        return lastRect;
    }

track_t Track::calculateDistance(const ObjectState &p) {
    Point_t diff = prediction - p.coord();
    return sqrtf(diff.x * diff.x + diff.y * diff.y);
}
