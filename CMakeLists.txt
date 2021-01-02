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

add_library(knn_result OBJECT lib/knn_result.cpp lib/knn_result.hpp)

if (ENABLE_CLANGTIDY)
    include(FindClangTidy.cmake)
endif ()

add_executable(knn main.cpp)
target_link_libraries(knn PUBLIC knn_lib)
add_executable(knn_v0 src/knn_v0.cpp)
target_link_libraries(knn_v0 PUBLIC knn_lib knn_result)
