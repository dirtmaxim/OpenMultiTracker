//
// Created by Ars Poyezzhayev on 21.11.16.
//

#include "OcclusionHandler.h"

bool OcclusionHandler::isMerge(const ObjectState &object1, const ObjectState &object2,
                               const ObjectState &potentialMerge) {
    return (object1.boundingBox() & potentialMerge.boundingBox()).area() > threshold &&
           (object2.boundingBox() & potentialMerge.boundingBox()).area() > threshold;
}

std::vector<cv::Rect> OcclusionHandler::findAllMerges(const std::vector<ObjectState> &detections) {
    std::vector<cv::Rect> merges;

    //Checks the collisions by pairs of the rectangles
    isMerges.resize(buffer.size());
    for (int l = 0; l < buffer.size(); ++l) {
        isMerges[l] = (false);
    }
    if (buffer.size() != 0 && detections.size() != 0) {
        for (int i = 0; i < buffer.size() - 1; ++i) {
            for (int j = i + 1; j < buffer.size(); ++j) {
                for (int k = 0; k < detections.size(); ++k) {
                    if (!(isMerges[i] || isMerges[j])) {
                        if (isMerge(buffer[i], buffer[j], detections[k])) {
                            merges.push_back(detections[k].boundingBox());
                            isMerges[i] = true;
                            isMerges[j] = true;
                        };
                    };
                }
            }
        }
    }
    isMerges.clear();
    return merges;
}

void OcclusionHandler::fillBuffer(std::vector<ObjectState> buffer) {
    this->buffer = buffer;
}

std::vector<cv::Rect> OcclusionHandler::findAllSplits(std::vector<cv::Rect> detections) {
    std::vector<cv::Rect> splits;
    if (buffer.size() != 0 && detections.size() != 0) {
        for (int i = 0; i < detections.size() - 1; ++i) {
            for (int j = i + 1; j < detections.size(); ++j) {
                for (int k = 0; k < buffer.size(); ++k) {
                    if (isMerge(detections[i], detections[j], buffer[k])) {
                        splits.push_back(buffer[k].boundingBox());
                    };
                }
            }
        }
    }
    return splits;
}

void OcclusionHandler::update(AssignmentsTable &assignments, const std::vector<ObjectState> &detections,
                              std::vector<std::unique_ptr<Track>> &tracks) {

    distMatrix_t intersectionArea(detections.size() * tracks.size());

    size_t N = tracks.size();
    size_t M = detections.size();

    // Fill the intersection area matrix
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


// ==========================Incomplete=================================
int calculateIntersection(cv::Rect blob1, cv::Rect blob2) {
    return (blob1 & blob2).area();
}

void findMerges(std::vector<Track> tracks, std::vector<cv::Rect> detections) {
    int intersections[tracks.size()][detections.size()];
    //form the intersections matrix
    for (int i = 0; i < tracks.size(); ++i) {
        for (int j = 0; j < detections.size(); ++j) {
            intersections[i][j] = (tracks[i].getLastRect() & detections[j]).area();
        }
    }

    //find merges
    for (int k = 0; k < tracks.size(); ++k) {
        for (int i = 0; i < detections.size(); ++i) {

        }
    }
}
