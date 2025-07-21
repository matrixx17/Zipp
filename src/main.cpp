#include <iostream>
#include "compressor.hpp"
#include "decompressor.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [c|d] input_file output_file\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string input_file = argv[2];
    std::string output_file = argv[3];

    if (mode == "c") {
        compress_file(input_file, output_file);
    } else if (mode == "d") {
        decompress_file(input_file, output_file);
    } else {
        std::cerr << "Invalid mode. Use 'c' for compression, 'd' for decompression.\n";
        return 1;
    }

    return 0;
}