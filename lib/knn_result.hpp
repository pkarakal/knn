#ifndef KNN_KNN_RESULT_HPP
#define KNN_KNN_RESULT_HPP


#include "knn_lib.hpp"
#include <algorithm>
#include <iostream>
#include <vector>


class KNNResult {
public:
    KNNResult(std::vector<int> neighborIndex,
              std::vector<double> neighborDistance, int m, int k);
    KNNResult(int m, int k);
    ~KNNResult() = default;
    void calculate_nearest_neighbors(std::vector<double>& x_indices, std::vector<double>& y_indices,
                                                int n, int mLen, int d, int k);

    int *getNidx() const;

    void setNidx(int *nidx);

    double *getNdist() const;

    void setNdist(double *ndist);

private:
    int *nidx{};
    std::vector<int> neighbor_index;
public:
    const std::vector<int> &getNeighborIndex() const;

    std::vector<double> &getNeighborDistance();

    void setNeighborDistance(const std::vector<double> &neighborDistance, int num);

private:
    double *ndist{};
    std::vector<double> neighbor_distance;
    int m;
    int k;
};


#endif //KNN_KNN_RESULT_HPP
