#ifndef KNN_KNN_LIB_HPP
#define KNN_KNN_LIB_HPP

#include <vector>

void quickSelect(double *arr, int l, int r, int k, int initialSize, std::vector<double> &values);
int partition(double *arr, int l, int r);

#endif //KNN_KNN_LIB_HPP
