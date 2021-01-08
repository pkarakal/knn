#include "lib/knn_lib.hpp"
namespace py= pybind11;

int main(int argc, char **argv){
    if(argc > 1){
        py::scoped_interpreter guard{};
        for(int i= 1; i < argc; ++i){
            convert_dok_to_csv(argv[i]);
        }
    } else {
        std::cerr <<"No file specified. Exiting ..." <<std::endl;
        std::cerr<<"Usage: " << argv[0] << " [ /path/to/file1 /path/to/file2 /path/to/fileN ]" <<std::endl;
    }
}