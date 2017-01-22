//
// Created by Ars Poyezzhayev on 22.01.17.
//

#ifndef HOCKEYPLAYERSRECOGNIZER_ASSIGNMENTSTABLE_H
#define HOCKEYPLAYERSRECOGNIZER_ASSIGNMENTSTABLE_H


#include <vector>
#include "../Utils/defines.h"
#include "DataAssociationAlgorithm.h"

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

/**
 * Class that represents the assignments matrix
 */


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


class AssignmentsTable {


public:
    track_t distanceThreshold;
    std::vector <Assignment> tracksToDetections;
    std::vector <Assignment> detectionsToTracks;

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

    AssignmentsTable (size_t tracksSize, size_t detectionsSize, track_t distanceThreshold);

    void solve(distMatrix_t &costMatrix, int N, int M);

    void assignTrackToDetection(int track, int detection);

    void merge(std::vector<int> &tracks, int detection);

    void split(std::vector<int> &detections, int track);


};


#endif //HOCKEYPLAYERSRECOGNIZER_ASSIGNMENTSTABLE_H
