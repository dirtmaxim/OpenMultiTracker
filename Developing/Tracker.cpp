//
// Created by Ars Poyezzhayev on 06.12.16.
//
#include "Tracker.h"
#include "AssignmentsTable.h"

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

    AssignmentsTable assignments(N, M, distanceThreshold);

    if (!tracks.empty()) {
        // Matrix of distance between N-th track and M-th detection.
        distMatrix_t Cost(N * M);

        /**
         * ================================================================================
         * Temporary collision detection
         * ================================================================================
         */
        // Check for collisions
        // Find merges on the video
//        std::vector<cv::Rect> merges = occlusionHandler.findAllMerges(detections);
//
//
//        // Find splits on the video
////        std::vector<cv::Rect> splits = occlusionHandler.findAllSplits(detections);
//        {
//            for (int i = 0; i < merges.size(); ++i) {
//                rectangle(*imgOutput, merges[i], cv::Scalar(0, 0, 255), 3, cv::LINE_4);
//            }
////
////        for (int i = 0; i < splits.size(); ++i) {
////            cv::rectangle(*imgOutput, splits[i], cv::Scalar(0, 255, 255), 3);
////        }
//
            // Show previous frames in green rectangles
            for (int i = 0; i < occlusionHandler.buffer.size(); ++i) {
                rectangle(*imgOutput, occlusionHandler.buffer[i].boundingBox(), cv::Scalar(0, 255, 0), 1);
            }
//        }
        /**
         * ================================================================================
         */

        // -----------------------------------
        // Step 1: Plot to Track Association. Fill the distance matrix with values
        // -----------------------------------


        for (size_t i = 0; i < tracks.size(); i++) {
            //Distance between prediction of the tracker and actual detection
            for (size_t j = 0; j < detections.size(); j++) {
                Cost[i + j * N] = tracks[i]->calculateDistance(detections[j]);
            }
        }

        // =========================== Add occlusion handling ==========================
        occlusionHandler.update(assignments, detections, tracks);


        assignments.solve(Cost, (int) N, (int) M);



        // -----------------------------------
        // Step 2: Track removal. If track didn't get detects long time, remove it.
        // -----------------------------------
////        for (int i = 0; i < static_cast<int>(tracks.size()); i++) {
////            if (tracks[i]->skipped_frames > maximum_allowed_skipped_frames) {
////                tracks.erase(tracks.begin() + i);
////                assignmentVector.erase(assignmentVector.begin() + i);
////                i--;
////            }
////        }
//        for (int i = 0; i < static_cast<int>(tracks.size()); i++) {
//            if (tracks[i]->skipped_frames > maximum_allowed_skipped_frames) {
//                assignments.tracksToDetections[i].type = Assignments::ToRemove;
//            }
//        }

        for (int i = 0; i < assignments.tracksToDetections.size(); ++i) {
            if (assignments.tracksToDetections[i].type == AssignmentsTable::Unassigned) {
                tracks[i]->skipped_frames++;
                if (tracks[i]->skipped_frames > maximum_allowed_skipped_frames) {
                    assignments.tracksToDetections[i].type = AssignmentsTable::ToRemove;
                }
            }
        }
    }

    // -----------------------------------
    // Step 3a: Track Initiation. Search for unassigned detects and start new tracks for them.
    // -----------------------------------
//    for (size_t i = 0; i < detections.size(); ++i) {
//        if (find(assignmentVector.begin(), assignmentVector.end(), i) == assignmentVector.end()) {
//            tracks.push_back(std::make_unique<Track>(detections[i], dt, Accel_noise_mag, NextTrackID++));
//        }
//    }
    for (size_t i = 0; i < detections.size(); ++i) {
        if (assignments.detectionsToTracks[i].type == AssignmentsTable::Unassigned) {
            tracks.push_back(std::make_unique<Track>(detections[i], dt, Accel_noise_mag, NextTrackID++));
        }
    }

    // -----------------------------------
    // Step 3b: Track Initiation and Removal for all the merged and splitted tracks.
    // -----------------------------------

    std::set<int> tracksToRemove;

    for (int i = 0; i < assignments.detectionsToTracks.size(); ++i) {
        int detectionType = assignments.detectionsToTracks[i].type;
        if (detectionType ==
            AssignmentsTable::Merged) { //add new track initialized by detection and merged tracks and delete all the merged tracks
            std::vector<int> relatedTracks;
            for (int track : assignments.detectionsToTracks[i].additionalAssignments) {
                relatedTracks.push_back(track);
                tracksToRemove.insert(track);
            }
            tracks.push_back(std::make_unique<Track>(detections[i], dt, Accel_noise_mag,
                                                     NextTrackID++, relatedTracks));
        } else if (detectionType >= 0){
            if (assignments.tracksToDetections[detectionType].type ==
                AssignmentsTable::Splitted) { //if the detection is assigned to track which is splitted then remove previous track and initialize new from it.
                tracks.push_back(std::make_unique<Track>(detections[i], dt, Accel_noise_mag, NextTrackID++, detectionType));
                tracksToRemove.insert(detectionType);
            }
        }
    }

    // Mark tracks to remove with label ToRemove
    for (int i : tracksToRemove) {
        assignments.tracksToDetections[i].type = AssignmentsTable::ToRemove;
    }

    // -----------------------------------
    // Step 4: Track Smoothing. Update Kalman Filter's state
    // -----------------------------------
//    for (size_t i = 0; i < assignmentVector.size(); i++) {
//        // If track updated less than one time, than filter state is not correct.
//        if (assignmentVector[i] != -1) // If we have assigned detect, then update the filter using its coordinates,
//        {
//            tracks[i]->skipped_frames = 0;
//            tracks[i]->update(detections[assignmentVector[i]], true, max_trace_length);
//        } else                     // if not continue using predictions of filter
//        {
//            tracks[i]->update(Point_t(), false, max_trace_length);
//        }
//    }
    for (size_t i = 0; i < assignments.tracksToDetections.size(); i++) {
        // If track updated less than one time, than filter state is not correct.
        int detectionType = assignments.tracksToDetections[i].type;
        if (detectionType >= 0) // If we have assigned detect, then update the filter using its coordinates,
        {
            tracks[i]->skipped_frames = 0;
            tracks[i]->update(detections[detectionType], true, max_trace_length);
        } else if (detectionType ==
                   AssignmentsTable::Unassigned)                  // if not continue using predictions of filter
        {
            tracks[i]->update(Point_t(), false, max_trace_length);
        }
    }

    // Delete all the tracks with label ToRemove
    for (int i = 0; i < assignments.tracksToDetections.size(); i++) {
        if (assignments.tracksToDetections[i].type == AssignmentsTable::ToRemove){
            tracks.erase(tracks.begin() + i);
            assignments.tracksToDetections.erase(assignments.tracksToDetections.begin() + i);
            i--;
        }
    }

    occlusionHandler.fillBuffer(detections);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
Tracker::~Tracker(void) {
}



/**
 * TODO:
 * 1. Add track constructor with related tracks
 * 2. Add update method for the occlusion handler
 * 3.
 */



