#ifndef KNN_KNN_LIB_HPP
#define KNN_KNN_LIB_HPP

#include <algorithm>
#include <openblas/cblas.h>
#include <tuple>
#include <vector>

void quickSelect(double *arr, int l, int r, int k, int initialSize, std::vector<double> &values);
int partition(double *arr, int l, int r);
void matrix_multiplication(double *A, double *B, std::tuple<int, int>& dimA,
                               std::tuple<int,int>& dimB, double *C);
static inline double computeSquare (double x);
void computeVectorSquare(std::vector<double>& vec);
void computeColumnSum(const double *arr, std::tuple<int,int> dims, std::vector<double>& vec);
void computeSumOfArrays(std::vector<double>& a, std::vector<double>& b, std::tuple<int,int> dimA,
                        std::tuple<int,int> dimB, std::vector<double>& vec);
void findIndices(std::vector<double> const& elements, std::vector<double> const& distances, std::vector<int>& indices);

#endif //KNN_KNN_LIB_HPP
