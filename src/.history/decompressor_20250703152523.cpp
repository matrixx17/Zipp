#include "decompressor.hpp"
#include "huffman.hpp"
#include "bitstream.hpp"
#include <fstream>
#include <unordered_map>

void decompress_file(const std::string& input_path, const std::string& output_path) {
    std::ifstream input(input_path, std::ios::binary);
    int map_size;
    input >> map_size;
    input.get();

    std::unordered_map<char, int> freq;
    for (int i = 0; i < map_size; ++i) {
        char ch;
        int f;
        input.get(ch);
        input >> f;
        input.get();
        freq[ch] = f;
    }

    auto root = build_huffman_tree(freq);
    BitReader reader(input);
    std::ofstream output(output_path, std::ios::binary);

    auto node = root;
    bool bit;
    while (reader.read_bit(bit)) {
        node = bit ? node->right : node->left;
        if (node->is_leaf()) {
            output.put(node->ch);
            node = root;
        }
    }
}