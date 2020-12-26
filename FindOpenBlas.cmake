##.rst:
## FindOpenBlas
## --------
## Finds the openBLAS library
##
## This will define the following variables::
##
## OpenBLAS_INCLUDE_DIR - the openBLAS include directories
## OpenBLAS_LIB - the openBLAS libraries

set(Open_BLAS_INCLUDE_SEARCH_PATHS
        /usr/include
        /usr/include/openblas
        /usr/include/openblas-base
        /usr/local/include
        /usr/local/include/openblas
        /usr/local/include/openblas-base
        /opt/OpenBLAS/include
        $ENV{OpenBLAS_HOME}
        $ENV{OpenBLAS_HOME}/include)

set(Open_BLAS_LIB_SEARCH_PATHS
        /lib/
        /lib/openblas-base
        /lib64/
        /usr/lib
        /usr/lib/openblas-base
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64
        /opt/OpenBLAS/lib
        $ENV{OpenBLAS}cd
        $ENV{OpenBLAS}/lib
        $ENV{OpenBLAS_HOME}
        $ENV{OpenBLAS_HOME}/lib)

find_path(OpenBLAS_INCLUDE_DIR NAMES
        cblas.h PATHS ${Open_BLAS_INCLUDE_SEARCH_PATHS})

find_library(OpenBLAS_LIB NAMES
        openblas PATHS ${Open_BLAS_LIB_SEARCH_PATHS})
