//
// Created by Ars Poyezzhayev on 21.11.16.
//

#include "OcclusionHandler.h"
#include "../Developing/DataAssociationAlgorithm.h"

bool OcclusionHandler::isMerge(const ObjectState &object1, const ObjectState &object2,
                               const ObjectState &potentialMerge) {
    return (object1.boundingBox() & potentialMerge.boundingBox()).area() > threshold && (object2.boundingBox() & potentialMerge.boundingBox()).area() > threshold;
}

std::vector<cv::Rect> OcclusionHandler::findAllMerges(const std::vector<ObjectState> &detections) {
    std::vector<cv::Rect> merges;

    //Checks the collisions by pairs of the rectangles
    isMerges.resize(buffer.size());
    for (int l = 0; l < buffer.size(); ++l) {
        isMerges[l] = (false);
    }
    if (buffer.size() != 0 && detections.size() != 0){
        for (int i = 0; i < buffer.size()-1; ++i) {
            for (int j = i+1; j < buffer.size(); ++j) {
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

std::vector<cv::Rect> OcclusionHandler::findAllSplits(std::vector<cv::Rect> blobs) {
    std::vector<cv::Rect> splits;
    if (buffer.size() != 0 && blobs.size() != 0){
        for (int i = 0; i < blobs.size()-1; ++i) {
            for (int j = i+1; j < blobs.size(); ++j) {
                for (int k = 0; k < buffer.size(); ++k) {
                    if (isMerge(blobs[i], blobs[j], buffer[k])){
                        splits.push_back(buffer[k].boundingBox());
                    };
                }
            }
        }
    }
    return splits;
}

void OcclusionHandler::update(std::vector<Track> tracks) {
    // Matrix of distance among N track predictions, for occlusion analysis
    size_t N = tracks.size();
    distMatrix_t CostPredictions(N * N);

    for (size_t i = 0; i < tracks.size(); i++)
    {
        for (size_t k = 0; k < tracks.size(); k++) {
            //Distance between predictions of different tracks
            CostPredictions[i + k * N] = tracks[i].calculateDistance(tracks[k].getPrediction());

            // Occlusion chance estimation. May be replaced with probabilistic function of occlusion estimation based
            // not only on the distance between predictions
            if (CostPredictions[i + k * N] > mergeThreshold){
                CostPredictions[i + k * N] = 1;
            } else {
                CostPredictions[i + k * N] = 0;
            }
            // end of occlusion chance estimation
        }
    }
}


// ==========================Incomplete=================================
int calculateIntersection(cv::Rect blob1, cv::Rect blob2){
    return (blob1 & blob2).area();
}

void findMerges(std::vector<Track> tracks, std::vector<cv::Rect> detections){
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
