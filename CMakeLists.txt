cmake_minimum_required(VERSION 3.17)
project(knn LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)

if (NOT VERBOSE)
    set(CMAKE_REQUIRED_QUIET ON)
endif ()

option(ENABLE_CLANGTIDY "Enable clang-tidy support?" OFF)
option(ENABLE_OPENBLAS "Enable openblas support?" ON)

if(ENABLE_OPENBLAS)
    include(FindOpenBlas.cmake)
endif()

add_library(knn_lib OBJECT lib/knn_lib.cpp lib/knn_lib.hpp)
target_link_libraries(knn_lib ${OpenBLAS_LIB})


if (ENABLE_CLANGTIDY)
    include(FindClangTidy.cmake)
endif ()

add_executable(knn main.cpp)
target_link_libraries(knn PUBLIC knn_lib)
