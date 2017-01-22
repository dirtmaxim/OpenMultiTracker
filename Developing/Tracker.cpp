//
// Created by Ars Poyezzhayev on 06.12.16.
//
#include "Tracker.h"

class Assignments {
    /**
     * It's optimized analogue of the matrix of assignments, e.g.
     *
     *          Tracks
     *        1   2   3   4   5   6   7
     *      ----------------------------
     * D  1|  *   A   *   *   *   *   * |
     * e  2|  *   *   *   *   *   *   * | (U)
     * t  3|  *   *   *   S   *   *   * |
     * e  4|  *   *   MS  *   M   *   * |
     * c  5|  M   *   *   *   *   M   * |
     * t  6|  *   *   *   S   *   *   * |
     * s  7|  *   *   S   *   *   *   * |
     *      ----------------------------
     *                                (U)
     *                                (R)
     * Where:
     * A - assigned track to detection (one-to-one)
     * U - unassigned track or detection (full line of *-s)
     * M - merged tracks (n tracks per one detection)
     * S - splitted track (n detections per one track)
     * MS - merge and split (track that is merged with other n tracks and simultaniously is splitted to m tracks)
     * R - track to remove
     * (*) - no assignment
     *
     * It's represented as tracksToDetections and detectionsToTracks:
     *                      1  2  3       4       5  6  7
     * tracksToDetections: {5, 1, S(4,7), S(3,6), 4, 5, U}
     *
     *                      1  2  3  4       5       6  7
     * detectionsToTracks: {2, U, 4, M(3,5), M(1,6), 4, 3}
     *
     * If tracks are splitted we should display it in the tracksToDetections list and if tracks are merged we consider it
     * in the detectionsToTracks list.
     */

    /**
     * Class that has a reference to the related assignment
     */
    class Assignment {
    public:
        int type;
        bool isConsidered;
        std::vector<int> additionalAssignments;

        Assignment(int type) : type(type) {}

    };

public:
    track_t distanceThreshold;
    std::vector<Assignment> tracksToDetections;
    std::vector<Assignment> detectionsToTracks;

    // special types of assignments. The Assigned type is not specific, it's just a number of an assignment
    enum type {
        Unassigned = -1,
        Merged = -2,
        Splitted = -3,
        ToRemove = -4
    };


public:
    /**
     * Transformation vectors maps elements of a matrix that contains only specific rows and columns of full matrix
     * to the corresponding elements of the full matrix.
     *
     * Full:
     *    1  2  3  4
     *   ------------
     * 1| A  B  C  D
     * 2| E  F  G  H
     * 3| J  K  L  M
     * 4| N  O  P  Q
     *
     * Partial:
     *       1(1)  2(3)  3(4)
     *      ----------------
     * 1(2)|  E     G     H
     * 2(4)|  N     P     Q
     *
     * Transformation vectors:
     * actualRows: [1] = 2, [2] = 4
     * actualCols: [1] = 1, [2] = 3, [3] = 4
     */

    Assignments(size_t tracksSize, size_t detectionsSize, track_t distanceThreshold) :
            distanceThreshold(distanceThreshold) {
        for (int i = 0; i < tracksSize; ++i) {
            tracksToDetections.push_back(Assignment(Unassigned));
        }

        for (int i = 0; i < detectionsSize; ++i) {
            detectionsToTracks.push_back(Assignment(Unassigned));
        }
    }

    void solve(distMatrix_t &costMatrix, int N, int M) {
        std::vector<int> actualRows; //positions of unassigned detections
        std::vector<int> actualCols; //positions of unassigned tracks

        //occlusion handling works with detectionsToTracks and tracksToDetections


        // Form transformation vectors
        for (int i = 0; i < detectionsToTracks.size(); ++i) {
            if (detectionsToTracks[i].type == Unassigned) {
                actualRows.push_back(i);
            }
        }

        for (int i = 0; i < tracksToDetections.size(); ++i) {
            if (tracksToDetections[i].type == Unassigned) {
                actualCols.push_back(i);
            }
        }

        // Build cost matrix (costAdj) only for unassigned tracks and detections
        size_t MAdj = actualRows.size();
        size_t NAdj = actualCols.size();
        distMatrix_t costAdj(NAdj * MAdj);

        for (int i = 0; i < MAdj; ++i) {
            for (int j = 0; j < NAdj; ++j) {
                costAdj[i * NAdj + j] = costMatrix[actualRows[i] * N + actualCols[j]];
            }
        }

        assignments_t assignmentVector;

        // Solving assignment problem (tracks and predictions of Kalman filter)
        DataAssociationAlgorithm HungarianAlg(Unassigned);
        HungarianAlg.solve(costAdj, NAdj, MAdj, assignmentVector, DataAssociationAlgorithm::many_forbidden_assignments);

        // -----------------------------------
        // Remove assignment from pairs with large distance
        // -----------------------------------
        for (size_t i = 0; i < assignmentVector.size(); i++) {
            if (assignmentVector[i] != Unassigned) {
                if (costAdj[i*NAdj + assignmentVector[i]] > distanceThreshold) {
                    assignmentVector[i] = Unassigned;
//                    tracksToDetections[actualCols[i]]->skipped_frames = 1;
                }
//            } else {
//                // If track have no assigned detect, then increment skipped frames counter.
//                assignmentVector[i] = Unassigned;
////                tracks[actualCols[i]]->skipped_frames++;
            }
        }

        // Data association algorithm produces one-to-one correspondence between tracks and detections. We need to incorporate these results into the assignments
        // After this step we have a full assignment matrix

        for (int i = 0; i < assignmentVector.size(); ++i) {
            if (assignmentVector[i] != Unassigned) {
                assignTrackToDetection(actualCols[i], actualRows[assignmentVector[i]]);
            }
        }

        /**
         * I.e.: 7 tracks and 7 detections:
         *                      1  2  3       4       5  6  7
         * tracksToDetections: {5, 1, S(4,7), S(3,6), 4, 5, U}
         *
         *                      1  2  3  4       5       6  7
         * detectionsToTracks: {2, U, 4, M(3,5), M(1,6), 4, 3}
         */

    }

    void assignTrackToDetection(int track, int detection) {
        tracksToDetections[track].type = detection;
        detectionsToTracks[detection].type = track;
    }

    void merge(std::vector<int> &tracks, int detection) {
        assert(tracks.size() > 1);
        //TODO: do we need the sorting for the tracks??

        for (int n : tracks) {
            tracksToDetections[n].type = detection;
            detectionsToTracks[detection].additionalAssignments.push_back(n);
        }
        detectionsToTracks[detection].type = Merged;
    }

    void split(std::vector<int> &detections, int track) {
        assert(detections.size() > 1);
        //TODO: do we need the sorting for the detections??
        for (int n : detections) {
            detectionsToTracks[n].type = track;
            tracksToDetections[track].additionalAssignments.push_back(n);
        }
        tracksToDetections[track].type = Splitted;
    }

    // Returns positions of the detections and tracks which are not in the occlusions in the initial Cost matrix
    //TODO: check the order of the detections and tracks. It could work only if the order is the same as in the initial matrix/
    void getOneToOneDetections(std::vector<int> &detectionsPositions, std::vector<int> &tracksPositions) {
        for (int i = 0; i < detectionsToTracks.size(); ++i) {
            if (detectionsToTracks[i].type == Unassigned) {
                detectionsPositions.push_back(i);
            }
        }
        for (int i = 0; i < tracksToDetections.size(); ++i) {
            if (tracksToDetections[i].type == Unassigned) {
                tracksPositions.push_back(i);
            }
        }
    }
};

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

    Assignments assignments(N, M, distanceThreshold);

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
//        occlusionHandler.update(tracks);

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
            if (assignments.tracksToDetections[i].type == Assignments::Unassigned) {
                tracks[i]->skipped_frames++;
                if (tracks[i]->skipped_frames > maximum_allowed_skipped_frames) {
                    assignments.tracksToDetections[i].type = Assignments::ToRemove;
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
        if (assignments.detectionsToTracks[i].type == Assignments::Unassigned) {
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
            Assignments::Merged) { //add new track initialized by detection and merged tracks and delete all the merged tracks
            std::vector<int> relatedTracks;
            for (int track : assignments.detectionsToTracks[i].additionalAssignments) {
                relatedTracks.push_back(track);
                tracksToRemove.insert(track);
            }
            tracks.push_back(std::make_unique<Track>(detections[i], dt, Accel_noise_mag,
                                                     NextTrackID++, relatedTracks)); //TODO: add track constructor with n tracks
        } else if (detectionType >= 0){
            if (assignments.tracksToDetections[detectionType].type ==
                Assignments::Splitted) { //if the detection is assigned to track which is splitted then remove previous track and initialize new from it.
                tracks.push_back(std::make_unique<Track>(detections[i], dt, Accel_noise_mag, NextTrackID++, detectionType));
                tracksToRemove.insert(detectionType);
            }
        }
    }

    // Mark tracks to remove with label ToRemove
    for (int i : tracksToRemove) {
        assignments.tracksToDetections[i].type = Assignments::ToRemove;
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
                   Assignments::Unassigned)                  // if not continue using predictions of filter
        {
            tracks[i]->update(Point_t(), false, max_trace_length);
        }
    }

    // Delete all the tracks with label ToRemove
    for (int i = 0; i < assignments.tracksToDetections.size(); i++) {
        if (assignments.tracksToDetections[i].type == Assignments::ToRemove){
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



