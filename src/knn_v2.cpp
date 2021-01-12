#include <chrono>
#include <iostream>
#include <openmpi-x86_64/mpi.h>
#include "../lib/knn_result.hpp"
#include "../lib/rapidcsv.h"
#include "../lib/vptree.hpp"

int main(int argc, char **argv){
    if (argc == 1) {
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
    MPI_Request req[6];
    MPI_Status stats[6];
    MPI_Status status;
    int processes, pid;
    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    int chunks = processes>1 ? n/processes : n;
    std::vector<double> X_local, Y, Z;
    int counter=0;
    KNNResult subKNN = KNNResult(chunks+1,k);
    std::vector<double> dists;
    std::vector<int> indices;
    X_local = std::vector<double>(chunks*d);
    std::copy(X.begin()+(pid*chunks*d), X.begin()+((pid+1)*chunks*d), X_local.begin());
    Y = std::vector<double>(chunks*d);
    Z = std::vector<double>(chunks*d);
    std::copy(X_local.begin(), X_local.end(), Y.begin());
    int next = pid +1, previous = pid-1;
    if(!pid){
        previous = processes-1;
    } else if (pid == processes -1){
        next = 0;
    }
    VPTree tree = VPTree(X_local, d,k);
    dists = std::vector<double>(k);
    indices = std::vector<int>(k);
    tree.query( X_local, &tree, d ,k, dists, indices);
    auto knn_Y_dist = std::vector<double>(chunks*k);
    auto knn_Y_ind = std::vector<int>(chunks*k);
    std::copy(std::begin(dists), std::end(dists), std::begin(knn_Y_dist));
    std::copy(std::begin(indices), std::end(indices), std::begin(knn_Y_ind));

    for(int i =0; i <processes; ++i){
        std::cout<<"Got here "<< i <<std::endl;

        MPI_Isend(&(Y.at(0)), chunks * d, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, &req[0]);
        MPI_Isend(&(knn_Y_dist.at(0)), chunks * k, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, &req[1]);
        MPI_Isend(&(knn_Y_ind.at(0)), chunks * k, MPI_INT, next, 0, MPI_COMM_WORLD, &req[2]);
        MPI_Irecv(&(Z.at(0)), chunks * d, MPI_DOUBLE, previous, 0, MPI_COMM_WORLD, &req[3]);
        MPI_Irecv(subKNN.getNdist(), chunks * d, MPI_DOUBLE, previous, 0, MPI_COMM_WORLD, &req[3]);
        MPI_Irecv(subKNN.getNidx(), chunks * d, MPI_INT, previous, 0, MPI_COMM_WORLD, &req[3]);
        KNNResult temp = KNNResult(chunks,k);
        if(!counter) {
            subKNN.calculate_nearest_neighbors(X_local, Y, chunks, chunks,d,k );
            std::cout<<"counter"<<std::endl;
            ++counter;
        } else {
            temp.calculate_nearest_neighbors(X_local, Y, chunks, chunks, d, k);
            std::cout<<"Got before setting neighbor"<<std::endl;
            subKNN.setNeighborDistance(temp.getNeighborDistance(), 0);
            temp.~KNNResult();
        }
        MPI_Waitall(2*processes, req, stats);
        std::copy(Z.begin(), Z.end(), Y.begin());
        std::cout<<"Got here"<<std::endl;

    }

    if(pid) {
        MPI_Send(&(subKNN.getNeighborDistance().at(0)), chunks * k, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }else {
        std::cout<<"Got here"<<std::endl;
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
    MPI_Finalize();
}

