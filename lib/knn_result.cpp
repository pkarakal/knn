#include "knn_result.hpp"


KNNResult::KNNResult(std::vector<int> neighborIndex,
                     std::vector<double> neighborDistance, int m, int k) :
                     neighbor_index(std::move(neighborIndex)),
                     neighbor_distance(std::move(neighborDistance)),
                     m(m), k(k) {
    this->ndist = neighborDistance.data();
    this-> nidx = neighborIndex.data();
}

KNNResult::KNNResult(int m, int k): m(m), k(k) {
    this->neighbor_index = std::vector<int>(m*k);
    this->neighbor_distance = std::vector<double>(m*k);
    this->nidx = this->neighbor_index.data();
    this->ndist = this->neighbor_distance.data();
}
/**
 *
 * @param x_indices
 * @param y_indices
 * @param n
 * @param mLen
 * @param d
 * @param k
 */
void KNNResult::calculate_nearest_neighbors(std::vector<double>& x_indices, std::vector<double>& y_indices,
                                            int n, int mLen, int d, int k) {
    // copy X and Y vectors to get squares later
    std::vector<double> x_square = std::vector<double>(x_indices.begin(), x_indices.end());
    std::vector<double> y_square = std::vector<double>(y_indices.begin(), y_indices.end());
    // reserve space for new vectors
    std::vector<double> x_summed = std::vector<double>(n);
    std::vector<double> y_summed = std::vector<double>(mLen);
    std::vector<double> D = std::vector<double>(n*mLen);
    std::tuple<int,int> x_dims = std::tuple<int,int>(n, d);
    std::tuple<int,int> y_dims = std::tuple<int,int>(mLen, d);
    // calculate x and y square
    computeVectorSquare(x_square);
    computeVectorSquare(y_square);
    // calculate x and y summed
    computeColumnSum(x_square.data(), x_dims, x_summed);
    computeColumnSum(y_square.data(), y_dims, y_summed);
    // calculate -2XY
    matrix_multiplication(x_indices.data(), y_indices.data(), x_dims, y_dims, D.data());
    computeSumOfArrays(x_summed, y_summed, x_dims, y_dims, D);
    // need a copy as quick select uses swap to sort the array first
    std::vector<double> _d = std::vector<double>(D);
    for(int i=0; i < mLen; ++i){
        std::vector<double> vec = std::vector<double>(D.begin()+ i*mLen, D.begin()+ (i+1)*mLen);
        std::vector<double> distances = std::vector<double>(k);
        quickSelect(vec.data(), 0, (int) vec.size() -1, 1, vec.size(), distances);
        std::copy(distances.begin(), distances.end(), this->neighbor_distance.begin()+(i*k));
    }
    findIndices(_d, this->neighbor_distance, this->neighbor_index);
}

const std::vector<int> &KNNResult::getNeighborIndex() const {
    return neighbor_index;
}


const std::vector<double> &KNNResult::getNeighborDistance() const {
    return neighbor_distance;
}

