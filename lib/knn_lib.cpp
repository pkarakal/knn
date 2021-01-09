#include "knn_lib.hpp"
namespace py=pybind11;

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
 * @param {std::vector<double>} values: A vector of the k smallest values
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

/**
 *
 * @param {double*} A: The matrix A
 * @param {double*} B: The matrix B
 * @param {std::tuple<int,int>} dimA: A tuple with the dimensions of A matrix
 * @param {std::tuple<int,int>} dimA: A tuple with the dimensions of B matrix
 * @param {double*} C: The result matrix
 */
void matrix_multiplication(double *A, double *B, std::tuple<int, int>& dimA,
                           std::tuple<int,int>& dimB, double *C) {
    assert(std::get<1>(dimA) == std::get<1>(dimB));
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans,
                std::get<0>(dimA), std::get<0>(dimB),
                std::get<1>(dimA), -2, A,
                std::get<1>(dimA), B, std::get<1>(dimA),
                0, C, std::get<0>(dimB));
}

static inline double computeSquare (double x) { return x*x; }

/**
 * Given a vector, calculates the square of the vector (A^2)
 * @param {std::vector<double>&} vec
 */
void computeVectorSquare(std::vector<double>& vec){
    std::transform(vec.begin(), vec.end(), vec.begin(), computeSquare);
}

/**
 *
 * @param {double*} arr: The matrix
 * @param {std::tuple<int,int>} dims: A tuple with the dimensions of arr matrix
 * @param {std::vector<double>&} C: A reference to the result vector
 */
void computeColumnSum(const double *arr, std::tuple<int,int> dims, std::vector<double>& vec){
    for(int i = 0; i < std::get<0>(dims); i++) {
        for(int j = 0; j < std::get<1>(dims); j++) {
            vec.at(i) += arr[std::get<1>(dims)*i + j];
        }
    }
}

/**
 *
 * @param {std::vector<double>&} a : reference to the first vector
 * @param {std::vector<double>&} b : reference to the second vector
 * @param {std::tuple<int,int>} dimA : a tuple with the dimensions of the first vector
 * @param {std::tuple<int,int>} dimB : a tuple with the dimensions of the second vector
 * @param {std::vector<double>&} vec : a reference to the result vector
 */
void computeSumOfArrays(std::vector<double>& a, std::vector<double>& b, std::tuple<int,int> dimA,
                        std::tuple<int,int> dimB, std::vector<double>& vec){
    int n = std::get<0>(dimA);
    int m = std::get<0>(dimB);
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            vec.at(j + m*i) = sqrt(vec.at(j + m*i) + a.at(i) + b.at(j));
        }
    }
}

void findIndices(std::vector<double> const& elements, std::vector<double> const& distances, std::vector<int>& indices){
    assert(distances.size() == indices.size());
    for (int i=0 ; i < distances.size(); ++i){
        indices.at(i) = std::distance(elements.begin(), std::find(elements.begin(), elements.end(), distances.at(i)));
    }
}

void convert_dok_to_csv(char* file){
    py::module_ convert = py::module_::import("sparse_matrix_to_csv");
    py::object result = convert.attr("convert_dictionary_to_csv")(file);
}

void preprocess_csv(char* file, int rows, char* delim){
    py::module_ convert = py::module_::import("preprocess_csvs");
    py::object result = convert.attr("preprocess_csvs")(file, rows, delim);
}

double fRand(double fMin, double fMax){
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}
