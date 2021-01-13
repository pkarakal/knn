#include <chrono>
#include <iostream>
#include <openmpi-x86_64/mpi.h>
#include "../lib/knn_result.hpp"
#include "../lib/rapidcsv.h"

KNNResult distAllKNN(double *X, int n, int d, int k);

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Wrong number of arguments. Exiting ..." << std::endl;
        std::cout << "Usage: " << argv[0] << " /path/to/file <number of neighbors to find>" << std::endl;
        std::cout << std::endl;
        exit(1);
    }
    // opening already parsed csv files
    // this also assumes that all mpi processes have access to the files
    // that will be processed and the same file hierarchy is required.
    rapidcsv::Document document = rapidcsv::Document();
    document.Load(argv[1]);
    int n = document.GetRowCount();
    int d = document.GetColumnCount();
    char *end;
    int k = strtol(argv[2], &end, 10);
    std::vector<double> X = std::vector<double>(n * d);
    // create a n*d vector from row vectors.
    for (int i = 0; i < n; ++i) {
        auto vec = document.GetRow<double>(i);
        std::copy(vec.begin(), vec.end(), X.begin() + (i * d));
        vec.clear();
    }
    MPI_Request req[2] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL};
    MPI_Status stats[2];
    MPI_Status status;
    int processes, pid;
    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    int chunks = processes>1 ? n/processes : n;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<double> X_local, Y, Z;
    int counter=0;
    KNNResult subKNN = KNNResult(chunks+1,k);
    X_local = std::vector<double>(chunks*d);
    std::copy(X.begin()+(pid*chunks*d), X.begin()+((pid+1)*chunks*d), X_local.begin());
    Y = std::vector<double>(chunks*d);
    Z = std::vector<double>(chunks*d);
    std::copy(X_local.begin(), X_local.end(), Y.begin());
    int next = pid +1, previous = pid-1;
    if(!pid)
        previous = processes-1;
    if (pid == processes -1)
        next = 0;
    for(int i =0; i <processes; ++i){
        MPI_Isend(&(Y.at(0)), chunks * d, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, &req[0]);
        MPI_Irecv(&(Z.at(0)), chunks * d, MPI_DOUBLE, previous, 0, MPI_COMM_WORLD, &req[1]);
        KNNResult temp = KNNResult(chunks,k);
        if(!counter) {
            subKNN.calculate_nearest_neighbors(X_local, Y, chunks, chunks,d,k );
            ++counter;
        } else {
            temp.calculate_nearest_neighbors(X_local, Y, chunks, chunks, d, k);
            subKNN.setNeighborDistance(temp.getNeighborDistance(), k);
        }
        MPI_Waitall(2*processes, req, stats);
        std::copy(Z.begin(), Z.end(), Y.begin());
    }

    if(pid) {
        MPI_Send(&(subKNN.getNeighborDistance().at(0)), chunks * k, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }else {
        KNNResult finalKNN = KNNResult(n,k);
        for(int i=1; i < processes; ++i) {
            auto temp = std::vector<double>(chunks*k);
            MPI_Recv(&(temp.at(0)), chunks*k, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            finalKNN.getNeighborDistance().insert(std::end(finalKNN.getNeighborDistance()), std::begin(temp), std::end(temp));
        }
        for (int i = 0; i < processes; ++i) {
            std::copy(subKNN.getNeighborDistance().begin(), subKNN.getNeighborDistance().end(), finalKNN.getNeighborDistance().begin());
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = stop - start;
    std::cout<<"Took "<< elapsed.count() << "s" << std::endl;
    MPI_Finalize();
}

