//
// Created by Ars Poyezzhayev on 21.11.16.
//

#ifndef HOCKEYPLAYERSRECOGNIZER_OCCLUSIONHANDLER_H
#define HOCKEYPLAYERSRECOGNIZER_OCCLUSIONHANDLER_H


#include <opencv2/core/types.hpp>
#include "Track.h"
#include "AssignmentsTable.h"

class OcclusionHandler {
private:
    int threshold = 100;
    float mergeThreshold = 100;
    std::vector<bool> isMerges;

public:
    std::vector<ObjectState> buffer;

    /**
     * The function cheсks whether 2 blobs merged to one or whether the merged blob is splitted to blobs. It is based on
     * area of intersection between blobs on 2 consecutive frames
     *
     * @param object1 - in previous frame for merge detection / in current frame for split detection
     * @param object2 - in previous frame for merge detection / in current frame for split detection
     * @param potentialMerge - in current frame for merge detection / in previous frame for split detection
     * @return true if merge or split occured, if there was no any merge then it returns false
     */
    bool isMerge(const ObjectState &object1, const ObjectState &object2, const ObjectState &potentialMerge);

    /**
     * Finds all merges on the frame according to the intersection rule
     * @param detections - current frame's blobs (measures)
     * @return - collection of all merges
     */
    std::vector<cv::Rect> findAllMerges(const std::vector<ObjectState> &detections);

    /**
     * Finds all splits on the frame according to the intersection rule
     * @param detections - current frame's blobs
     * @return - collcection of all merged blobs of previous frame that were splitted in the current one
     */
    std::vector<cv::Rect> findAllSplits(std::vector<cv::Rect> detections);

    /**
     * Fills  buffer with blobs
     * @param buffer
     */
    void fillBuffer(std::vector<ObjectState> buffer);

    void update(AssignmentsTable &assignments, const std::vector<ObjectState> &detections, std::vector<std::unique_ptr<Track>> &tracks);
};


#endif //HOCKEYPLAYERSRECOGNIZER_OCCLUSIONHANDLER_H
