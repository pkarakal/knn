#include "../lib/knn_lib.hpp"
#include "../lib/knn_result.hpp"
#include <random>
#include <iostream>

int main() {
    int n = 100;
    int d = 20;
    int m = 4;
    int k = 5;
    KNNResult knn = KNNResult(m,k);
    srand(time(nullptr));
    std::vector<double> x = std::vector<double>(n*d);
    std::vector<double> y = std::vector<double>(m*d);
    for(int i=0; i < n*d; ++i) {
        x.at(i) = fRand(0, 100);
    }
    for(int i =0; i < m*d; ++i) {
        y.at(i) = fRand(0, 100);
    }
    knn.calculate_nearest_neighbors(x, y, n, m, d, k);
}