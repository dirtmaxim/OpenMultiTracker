//
// Created by Ars Poyezzhayev on 22.01.17.
//

#include "AssignmentsTable.h"

AssignmentsTable::AssignmentsTable(size_t tracksSize, size_t detectionsSize, track_t distanceThreshold) :
        distanceThreshold(distanceThreshold) {
    for (int i = 0; i < tracksSize; ++i) {
        tracksToDetections.push_back(Assignment(Unassigned));
    }

    for (int i = 0; i < detectionsSize; ++i) {
        detectionsToTracks.push_back(Assignment(Unassigned));
    }
}

void AssignmentsTable::solve(distMatrix_t &costMatrix, int N, int M) {
    std::vector<int> actualRows; //positions of unassigned detections
    std::vector<int> actualCols; //positions of unassigned tracks

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


    // Form transformation vectors
    // Returns positions of the detections and tracks which are not in the occlusions in the initial Cost matrix
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
            if (costAdj[i + assignmentVector[i] * NAdj] > distanceThreshold) {
                assignmentVector[i] = Unassigned;
            }
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

void AssignmentsTable::assignTrackToDetection(int track, int detection) {
    tracksToDetections[track].type = detection;
    detectionsToTracks[detection].type = track;
}

void AssignmentsTable::merge(std::vector<int> &tracks, int detection) {
    assert(tracks.size() > 1);
    //TODO: do we need the sorting for the tracks??
    for (int n : tracks) {
        if (tracksToDetections[n].type == Unassigned) {
            tracksToDetections[n].type = detection;
        } else if (tracksToDetections[n].type >= 0) { // If the track has detections assigned already
            tracksToDetections[n].additionalAssignments.push_back(tracksToDetections[n].type);
            tracksToDetections[n].type = Splitted;
            tracksToDetections[n].additionalAssignments.push_back(detection);
        } else if (tracksToDetections[n].type == Splitted) { //If the track is already splitted
            tracksToDetections[n].additionalAssignments.push_back(detection);
        }
        detectionsToTracks[detection].additionalAssignments.push_back(n);
    }
    detectionsToTracks[detection].type = Merged;
}

void AssignmentsTable::split(std::vector<int> &detections, int track) {
    assert(detections.size() > 1);
    //TODO: do we need the sorting for the detections??
    for (int n : detections) {
        if (detectionsToTracks[n].type == Unassigned){
            detectionsToTracks[n].type = track;
        } else if (detectionsToTracks[n].type >= 0) {
            detectionsToTracks[n].additionalAssignments.push_back(detectionsToTracks[n].type);
            detectionsToTracks[n].type = Merged;
            detectionsToTracks[n].additionalAssignments.push_back(track);
        } else if (detectionsToTracks[n].type == Merged){
            detectionsToTracks[n].additionalAssignments.push_back(track);
        }
        tracksToDetections[track].additionalAssignments.push_back(n);
    }
    tracksToDetections[track].type = Splitted;
}
