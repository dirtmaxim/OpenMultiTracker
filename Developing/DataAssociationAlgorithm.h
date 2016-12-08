//
// Created by Ars Poyezzhayev on 06.12.16.
//

#ifndef HOCKEYPLAYERSRECOGNIZER_DATAASSOCIATIONALGORITHM_H
#define HOCKEYPLAYERSRECOGNIZER_DATAASSOCIATIONALGORITHM_H
#include <vector>
#include <iostream>
#include <limits>
#include <time.h>
#include "../Utils/defines.h"
typedef std::vector<track_t> distMatrix_t;
typedef std::vector<int> assignments_t;

class DataAssociationAlgorithm {
private:
    // --------------------------------------------------------------------------
    // Computes the optimal assignment (minimum overall costs) using Munkres algorithm.
    // --------------------------------------------------------------------------
    void assignmentOptimal(assignments_t &assignment, track_t &cost, const distMatrix_t &distMatrixIn, size_t nOfRows,
                           size_t nOfColumns);
    void buildassignmentvector(assignments_t& assignment, bool *starMatrix, size_t nOfRows, size_t nOfColumns);
    void computeassignmentcost(const assignments_t& assignment, track_t& cost, const distMatrix_t& distMatrixIn, size_t nOfRows);
    void step2a(assignments_t& assignment, track_t *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim);
    void step2b(assignments_t& assignment, track_t *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim);
    void step3(assignments_t& assignment, track_t *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim);
    void step4(assignments_t& assignment, track_t *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim, size_t row, size_t col);
    void step5(assignments_t& assignment, track_t *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim);
    // --------------------------------------------------------------------------
    // Computes a suboptimal solution. Good for cases with many forbidden assignments.
    // --------------------------------------------------------------------------
    void assignmentSuboptimal1(assignments_t &assignment, track_t &cost, const distMatrix_t &distMatrixIn,
                               size_t nOfRows, size_t nOfColumns);
    // --------------------------------------------------------------------------
    // Computes a suboptimal solution. Good for cases with many forbidden assignments.
    // --------------------------------------------------------------------------
    void assignmentSuboptimal2(assignments_t &assignment, track_t &cost, const distMatrix_t &distMatrixIn,
                               size_t nOfRows, size_t nOfColumns);

public:
    enum TMethod
    {
        optimal,
        many_forbidden_assignments,
        without_forbidden_assignments
    };

    DataAssociationAlgorithm();
    ~DataAssociationAlgorithm();

    track_t solve(const distMatrix_t &distMatrixIn, size_t nOfRows, size_t nOfColumns, assignments_t &assignment,
                  TMethod Method = optimal);
};

#endif //HOCKEYPLAYERSRECOGNIZER_DATAASSOCIATIONALGORITHM_H
