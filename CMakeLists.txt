cmake_minimum_required(VERSION 3.17)
project(knn LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)

if (NOT VERBOSE)
    set(CMAKE_REQUIRED_QUIET ON)
endif ()

find_package(PkgConfig)

option(ENABLE_CLANGTIDY "Enable clang-tidy support?" OFF)
option(ENABLE_OPENBLAS "Enable openblas support?" ON)
option(ENABLE_PYBIND11 "Enable python bindings support?" ON)
option(ENABLE_OPENMPI "Enable OpenMPI support?" ON)

if(ENABLE_OPENBLAS)
    include(FindOpenBlas.cmake)
endif()

if(ENABLE_PYBIND11)
    find_package(pybind11 REQUIRED)
    link_libraries(pybind11::embed pybind11::module pybind11::lto)
endif()

if(ENABLE_OPENMPI)
    include(FindOpenMPI.cmake)
endif()

add_library(knn_lib OBJECT lib/knn_lib.cpp lib/knn_lib.hpp)
target_link_libraries(knn_lib PRIVATE ${OpenBLAS_LIB})

add_library(knn_result OBJECT lib/knn_result.cpp lib/knn_result.hpp)
add_library(rapidcsv OBJECT lib/rapidcsv.h)

if (ENABLE_CLANGTIDY)
    include(FindClangTidy.cmake)
endif ()

add_compile_options(-g)
add_executable(knn main.cpp)
target_link_libraries(knn PUBLIC knn_lib)
add_executable(knn_v0 src/knn_v0.cpp)
target_link_libraries(knn_v0 PUBLIC knn_lib knn_result)
add_executable(converter converter.cpp)
target_link_libraries(converter PUBLIC knn_lib knn_result)
add_executable(preprocessor preprocessor.cpp)
target_link_libraries(preprocessor PUBLIC knn_lib knn_result)
if(OPEN_MPI_VERSION)
    add_executable(knn_v1 src/knn_v1.cpp)
    target_link_libraries(knn_v1 PUBLIC knn_lib knn_result rapidcsv ${OPEN_MPI_LIBRARIES})
    add_compile_options(-pthread)
    if(OPEN_MPI_COMPILE_FLAGS)
        set_target_properties(knn_v1 PROPERTIES COMPILE_FLAGS "${OPEN_MPI_COMPILE_FLAGS}")
    endif()
    if(OPEN_MPI_LINK_FLAGS)
        set_target_properties(knn_v1 PROPERTIES LINK_FLAGS "${OPEN_MPI_LINK_FLAGS}")
    endif()

endif()
