#.rst:
# FindOpenMPI
# --------
# Finds the OpenMPI library
#
# This will define the following variables::
#
# OPEN_MPI_INCLUDE_DIRS - the OPEN_MPI include directories
# OPEN_MPI_LIBRARIES - the OPEN_MPI libraries

if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_OMPI ompi QUIET)
    find_library(OPEN_MPI_LIBRARIES
            NAMES mpi
            PATHS ${PC_OMPI_LIBDIR})
    find_path(OPEN_MPI_INCLUDE_DIR NAMES mpi.h
            PATHS ${PC_OMPI_INCLUDEDIR})
    include_directories(${PC_OMPI_INCLUDEDIR} ${PC_OMPI_PREFIX} ${PC_OMPI_LIBDIR})
    set(OPEN_MPI_VERSION ${PC_OMPI_VERSION})
    set(OPEN_MPI_COMPILE_FLAGS ${PC_OMPI_CFLAGS_OTHER})
    message("-- Found OpenMPI: ${PC_OMPI_PREFIX} (found version ${PC_OMPI_VERSION})")
else()
    find_package(MPI REQUIRED)
    message("-- Found MPICXX: ${MPI_CXX_COMPILER} (found version ${MPI_CXX_VERSION}")
    set(OPEN_MPI_INCLUDE_DIRS ${MPI_INCLUDE_PATH})
    set(OPEN_MPI_LIBRARIES ${MPI_LIBRARIES})
    set(OPEN_MPI_VERSION ${MPI_CXX_VERSION})
    set(OPEN_MPI_COMPILE_FLAGS ${MPI_COMPILE_FLAGS})
    set(OPEN_MPI_LINK_FLAGS ${MPI_LINK_FLAGS})
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
endif()

mark_as_advanced(OPEN_MPI_INCLUDE_DIRS OPEN_MPI_LIBRARIES OPEN_MPI_COMPILE_FLAGS OPEN_MPI_LINK_FLAGS)