# KNN (K-nearest neighbors)
C++ implementation of K-nearest neighbors using MPI. This project is implemented
as a part of a homework exercise for [050] - Parallel & Distributed Systems course
of ECE Department, AUTh.

## Getting Started

### Prerequisites
1. CMake
2. Make
3. g++
4. OpenMPI
5. OpenBLAS
6. pkgconf (Optional)

To install them on variant Linux distributions follow the instructions below

#### Fedora
```shell
$ sudo dnf upgrade --refresh # updates installed packages and repositories metadata
$ sudo dnf install cmake make gcc-c++ openmpi  python3 \
  openmpi-devel openblas openblas-devel pkgconf python3-pybind11
# replace ${arch} with your CPU architecture eg .x86-64, aamd64
$ module load mpi/openmpi-${arch}
```

#### Ubuntu
```shell
# updates installed packages and repositories metadata
$ sudo apt-get update && sudo apt-get upgrade
#installs dependencies
$ sudo apt-get install cmake make g++ libopenmpi-dev \ 
  libopenblas-dev pkg-config python3 python-pybind11 
```



### Instructions
1.  Clone the repository
    ```shell script
    $ git clone git@github.com:pkarakal/knn.git
    ```
2.  Go to that directory
    ```shell script
    $ cd knn/
    ```
3.  a. Generate Makefiles from the CMakefiles.txt
    ```shell script
    $ cmake -S .
    ```
    b. There are additional cmake files that import OpenBLAS and OpenMPI and 
    change the compiler to use the OpenMPI g++ compiler and there is also the
    option  to include ClangTidy support.
    To enable the build of the executable that leverages OpenBLAS run
    ```shell script
    $ cmake -S . -DENABLE_OPENBLAS=ON
    ``` 
    To enable the build of the executable that leverages OpenMPI run
    ```
    $ cmake -S . -DENABLE_OPENMPI=ON -DCMAKE_CXX_COMPILER=$(command -v mpic++)
    ```
    `mpic++` is a wrapper for g++ that already utilizes the corrent compiler and
    linker flags.
    Alternatively, if you don't want to change the compiler for all the executables
    and you already have `mpi`in your path, you can apply the patch present in the
    git tree which adds a custom cmake target that builds the executables in the
    same way but is more verbose
    ```shell
    $ git apply ./cmake.patch
    $ cmake -S . -DENABLE_OPENMPI=ON
    ```
    
    To enable Clang Tidy support use the following flag
    ```shell script
    $ cmake -S . -DENABLE_CLANGTIDY=ON
    ```
4.  Build and run the application
    ```shell script
    $ cmake --build . && ./knn_v${variant}
    ```
5.  To execute multiple instances of the program on your current run-tme environment
    you can use `mpirun` command like so
    ```shell
    $ mpirun -n <number of instances> ./knn_v${variant}
    ```
6.  There is also a converter program that converts files in dictionary of keys 
    format `(col:data)` to csv excluding the "Label" column. This is calls python 
    under the hood for reading and converting to csv. To compile it run 
    ```shell
    $ pip3 install -r requirements.txt
    $ cmake -S . -DENABLE_PYBIND11=ON
    $ make converter
    ```
    To run it use the following format:
    ```shell
    $ ./converter /path/to/file 
    # or for multiple files
    $ ./converter [ /path/to/file1 /path/to/file2 /path/to/fileN ]
    ```
    Make sure you have a python >=3.6 interpreter installed and in your PATH. 