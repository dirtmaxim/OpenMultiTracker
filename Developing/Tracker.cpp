//
// Created by Ars Poyezzhayev on 06.12.16.
//

#include "Tracker.h"

// ---------------------------------------------------------------------------
// Tracker manager tracks. Create, remove, update.
// ---------------------------------------------------------------------------
Tracker::Tracker(
        track_t dt_,
        track_t Accel_noise_mag_,
        track_t dist_thres_,
        size_t maximum_allowed_skipped_frames_,
        size_t max_trace_length_
) :
        dt(dt_),
        Accel_noise_mag(Accel_noise_mag_),
        distanceThreshold(dist_thres_),
        maximum_allowed_skipped_frames(maximum_allowed_skipped_frames_),
        max_trace_length(max_trace_length_),
        NextTrackID(0),
        occlusionHandler(OcclusionHandler()) {
}

/**
 * Default contructor
 */
Tracker::Tracker() {}

// ---------------------------------------------------------------------------
//                                  Methods
// ---------------------------------------------------------------------------
void Tracker::update(
        const std::vector<ObjectState> &detections,
        cv::Mat *imgOutput) {

    size_t N = tracks.size();        // tracks
    size_t M = detections.size();    // detects

    assignments_t assignment;

    if (!tracks.empty()) {
        // Matrix of distance between N-th track and M-th detection.
        distMatrix_t Cost(N * M);

        // Check for collisions
        // Find merges on the video
        std::vector<cv::Rect> merges = occlusionHandler.findAllMerges(detections);


        // Find splits on the video
//        std::vector<cv::Rect> splits = occlusionHandler.findAllSplits(detections);

        for (int i = 0; i < merges.size(); ++i) {
            cv::rectangle(*imgOutput, merges[i], cv::Scalar(0, 0, 255), 3, cv::LINE_4);
        }
//
//        for (int i = 0; i < splits.size(); ++i) {
//            cv::rectangle(*imgOutput, splits[i], cv::Scalar(0, 255, 255), 3);
//        }

        // Show previous frames in green rectangles
        for (int i = 0; i < occlusionHandler.buffer.size(); ++i) {
            cv::rectangle(*imgOutput, occlusionHandler.buffer[i].boundingBox(), cv::Scalar(0, 255, 0), 1);
        }

        // -----------------------------------
        // Step 1: Plot to Track Association. Fill the distance matrix with values
        // -----------------------------------


        for (size_t i = 0; i < tracks.size(); i++) {
            //Distance between prediction of the tracker and actual detection
            for (size_t j = 0; j < detections.size(); j++) {
                Cost[i + j * N] = tracks[i]->calculateDistance(detections[j]);
            }
            // =========================== Add occlusion handling ==========================

        }
        // Find merges on the video



        // Solving assignment problem (tracks and predictions of Kalman filter)
        DataAssociationAlgorithm HungarianAlg;
        HungarianAlg.solve(Cost, N, M, assignment, DataAssociationAlgorithm::many_forbidden_assignments);

        // -----------------------------------
        // Remove assignment from pairs with large distance
        // -----------------------------------
        for (size_t i = 0; i < assignment.size(); i++) {
            if (assignment[i] != -1) {
                if (Cost[i + assignment[i] * N] > distanceThreshold) {
                    assignment[i] = -1;
                    tracks[i]->skipped_frames = 1;
                }
            } else {
                // If track have no assigned detect, then increment skipped frames counter.
                tracks[i]->skipped_frames++;
            }
        }

        // -----------------------------------
        // Step 2: Track removal. If track didn't get detects long time, remove it.
        // -----------------------------------
        for (int i = 0; i < static_cast<int>(tracks.size()); i++) {
            if (tracks[i]->skipped_frames > maximum_allowed_skipped_frames) {
                tracks.erase(tracks.begin() + i);
                assignment.erase(assignment.begin() + i);
                i--;
            }
        }
        // ===================== Occlusion handling ============================
    }

    // -----------------------------------
    // Step 3: Track Initiation. Search for unassigned detects and start new tracks for them.
    // -----------------------------------
    for (size_t i = 0; i < detections.size(); ++i) {
        if (find(assignment.begin(), assignment.end(), i) == assignment.end()) {
            tracks.push_back(std::make_unique<Track>(detections[i], dt, Accel_noise_mag, NextTrackID++));
        }
    }

    // -----------------------------------
    // Step 4: Track Smoothing. Update Kalman Filter's state
    // -----------------------------------
    for (size_t i = 0; i < assignment.size(); i++) {
        // If track updated less than one time, than filter state is not correct.
        if (assignment[i] != -1) // If we have assigned detect, then update the filter using its coordinates,
        {
            tracks[i]->skipped_frames = 0;
            tracks[i]->update(detections[assignment[i]], true, max_trace_length);
        } else                     // if not continue using predictions of filter
        {
            tracks[i]->update(Point_t(), false, max_trace_length);
        }
    }

    occlusionHandler.fillBuffer(detections);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
Tracker::~Tracker(void) {
}



