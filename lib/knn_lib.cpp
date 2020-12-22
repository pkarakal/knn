#include "knn_lib.hpp"
#include <bits/stdc++.h>

int partition(double *arr, int l, int r){
    double x = arr[r];
    int i = l;
    for (int j = l; j <= r - 1; j++) {
        if (arr[j] <= x) {
            std::swap(arr[i], arr[j]);
            i++;
        }
    }
    std::swap(arr[i], arr[r]);
    return i;
}

/**
 * Given an array search for the smallest entries. The number of elements is
 * bound by the size of the values vector. When the kth smallest is found the
 * function is being called recursively for the next entry.
 * @param {double[]} arr: the array given to search the kth smallest
 * @param {int} l: The left index
 * @param {int} r: The right index
 * @param {int} k: The kth smallest element
 * @param {int} initialSize: The initial size of the array
 * @param {std::vectpr<double>} values: A vector of the k smallest values
 */
void quickSelect(double arr[], int l, int r, int  k, int initialSize, std::vector<double> &values) {
    // If k is smaller than number of
    // elements in array
    if (k > 0 && k <= r - l + 1 && k<= values.size()) {
        // Partition the array around last
        // element and get position of pivot
        // element in sorted array
        int index = partition(arr, l, r);
        // If position is same as k
        if (index - l == k - 1) {
            values.at(k-1) = arr[index];
            return quickSelect(arr, 0, initialSize-1, k+1, initialSize, values);
        }
        // If position is more, recur
        // for left subarray
        if (index - l > k - 1)
            return quickSelect(arr, l, index - 1, k, initialSize, values);

        // Else recur for right subarray
        return quickSelect(arr, index + 1, r,
                           k - index + l - 1, initialSize, values);
    }
}
