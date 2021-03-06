#include <chrono>
#include <iostream>
#include <fstream>
#include <openmpi-x86_64/mpi.h>
#include "../lib/knn_result.hpp"
#include "../lib/rapidcsv.h"
#include "../lib/knn_lib.hpp"

int main(int argc, char **argv){
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
    MPI_Request req[6] {MPI_REQUEST_NULL, MPI_REQUEST_NULL, MPI_REQUEST_NULL, MPI_REQUEST_NULL, MPI_REQUEST_NULL, MPI_REQUEST_NULL};
    MPI_Status stats[6];
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
    auto vpTree = (VPTree*) malloc(sizeof (VPTree));
    vpTree = vpt_create(X_local.data(), vpTree, d,k);
    VPTree * current_node = vpTree;
    dists = std::vector<double>(k);
    indices = std::vector<int>(k);
    auto queryVec = std::vector<double>(d,0);
    for(int i = 0; i < chunks; i++) {
        for(int j = 0; j < d; j++) {
            queryVec.at(j) = X_local.at(i * d + j);
        }
        search_vpt(queryVec.data(), vpTree, d, k, dists.data(), indices.data());
        for(int j = 0; j < k; j++) {
            subKNN.getNdist()[j * chunks + i] = dists.at(j);
            subKNN.getNidx()[j * chunks + i] = indices.at(j);
        }
    }
    auto knn_Y_dist = std::vector<double>(chunks*k);
    auto knn_Y_ind = std::vector<int>(chunks*k);
    std::copy(std::begin(dists), std::end(dists), std::begin(knn_Y_dist));
    std::copy(std::begin(indices), std::end(indices), std::begin(knn_Y_ind));

    for(int i =0; i <processes; ++i){
        !i ? std::cout<< "Started calculations" << std::endl : std::cout<< "Exchanged data" <<std::endl;
        MPI_Isend(&(Y.at(0)), chunks * d, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, &req[0]);
        MPI_Isend(&(knn_Y_dist.at(0)), chunks * k, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, &req[1]);
        MPI_Isend(&(knn_Y_ind.at(0)), chunks * k, MPI_INT, next, 0, MPI_COMM_WORLD, &req[2]);
        MPI_Irecv(&(Z.at(0)), chunks * d, MPI_DOUBLE, previous, 0, MPI_COMM_WORLD, &req[3]);
        MPI_Irecv(subKNN.getNdist(), chunks * k, MPI_DOUBLE, previous, 0, MPI_COMM_WORLD, &req[4]);
        MPI_Irecv(subKNN.getNidx(), chunks * k, MPI_INT, previous, 0, MPI_COMM_WORLD, &req[5]);
        KNNResult temp = KNNResult(chunks,k);
        if(!counter) {
            subKNN.calculate_nearest_neighbors(X_local, Y, chunks, chunks,d,k );
            ++counter;
        } else {
            temp.calculate_nearest_neighbors(X_local, Y, chunks, chunks, d, k);
            subKNN.setNeighborDistance(temp.getNeighborDistance(), k);
        }
        for(int w = 0; w < chunks; w++) {
            for(int j = 0; j < d; j++) {
                queryVec.at(j) = Y.at(w * d + j);
            }
            search_vpt(queryVec.data(), vpTree, d, k, dists.data(), indices.data());
            for(int j = 0; j < k; j++) {
                subKNN.getNdist()[j * chunks + w] = dists.at(j);
                subKNN.getNidx()[j * chunks + w] = indices.at(j);
            }
        }
        MPI_Waitall(6, req, stats);
        std::copy(Z.begin(), Z.end(), Y.begin());
        std::copy(subKNN.getNeighborDistance().begin(), subKNN.getNeighborDistance().end(), dists.begin());
        std::copy(subKNN.getNeighborIndex().begin(), subKNN.getNeighborIndex().end(), indices.begin());
    }
    KNNResult finalKNN = KNNResult(n,k);
    if(pid) {
        MPI_Send(&(subKNN.getNeighborDistance().at(0)), chunks * k, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }else {
        for(int i=1; i < processes; ++i) {
            auto temp = std::vector<double>(chunks*k);
            MPI_Recv(&(temp.at(0)), chunks*k, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            finalKNN.getNeighborDistance().insert(std::end(finalKNN.getNeighborDistance()), std::begin(temp), std::end(temp));
        }
        std::copy(subKNN.getNeighborDistance().begin(), subKNN.getNeighborDistance().end(), finalKNN.getNeighborDistance().begin());
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = stop - start;
    std::cout<<"Took "<< elapsed.count() << "s" << std::endl;
    MPI_Finalize();
    if(!pid){
        std::string input = argv[1];
        std::string file_name = "results_" + input + "results_" + "v2.txt";
        std::fstream file ("./results/v2/" + file_name);
        if(file.is_open()){
            file << "Took " << elapsed.count()<<"s"<<std::endl;
            file << "K: " << k <<std::endl;
            file << "N: " << n <<std::endl;
            for (int i=0; i < X.size(); ++i){
                file << i << ": ";
                for(int j=0; j <k; ++j){
                    file << finalKNN.getNeighborIndex().at(i*k+j) << " ";
                }
                file <<std::endl;
            }
            file.close();
        }
    }
}

