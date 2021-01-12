#include "lib/knn_lib.hpp"
namespace py= pybind11;

int main(int argc, char **argv){
    if(argc > 1){
        py::scoped_interpreter guard{};
        for(int i= 1; i < argc; i+=3){
            char *end;
            preprocess_csv(argv[i], strtol(argv[i+1], &end, 10), argv[i+2]);
        }
    } else {
        std::cerr <<"No file specified. Exiting ..." <<std::endl;
        std::cerr<<"Usage: " << argv[0] << " /path/to/file1 <rows_to_skip> \"regex_of_delim\"" <<std::endl;
    }
}