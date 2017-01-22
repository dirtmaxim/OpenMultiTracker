//
// Created by Ars Poyezzhayev on 21.11.16.
//

#include "OcclusionHandler.h"

void OcclusionHandler::fillBuffer(std::vector<ObjectState> buffer) {
    this->buffer = buffer;
}

void OcclusionHandler::update(AssignmentsTable &assignments, const std::vector<ObjectState> &detections,
                              std::vector<std::unique_ptr<Track>> &tracks) {

    distMatrix_t intersectionArea(detections.size() * tracks.size());

    size_t N = tracks.size();
    size_t M = detections.size();

    // Fill the intersection area matrix. It contains the pairwise interesection area of all the detections and tracks.
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            intersectionArea[i + j * N] = (tracks[i]->getLastRect() & detections[j].boundingBox()).area();
        }
    }

    // Find merges - go through rows
    for (int i = 0; i < M; i++) {
        std::vector<int> tracksMerged;
        for (int j = 0; j < N; j++) {
            if (intersectionArea[i * N + j] > mergeThreshold) tracksMerged.push_back(j);
        }
        if(tracksMerged.size() > 1){
            assignments.merge(tracksMerged, i);
        }
    }

    // Find splits - go through cols
    for (int i = 0; i < N; i++) {
        std::vector<int> detectionsOfSplittedTrack;
        for (int j = 0; j < M; j++) {
            if (intersectionArea[i + j*N] > mergeThreshold) detectionsOfSplittedTrack.push_back(j);
        }
        if(detectionsOfSplittedTrack.size() > 1){
            assignments.split(detectionsOfSplittedTrack, i);
        }
    }
}
