// #include "decompressor.hpp"
// #include "huffman.hpp"
// #include "bitstream.hpp"
// #include <fstream>
// #include <unordered_map>

// void decompress_file(const std::string& input_path, const std::string& output_path) {
//     std::ifstream input(input_path, std::ios::binary);
//     int map_size;
//     input >> map_size;
//     input.get();

//     std::unordered_map<char, int> freq;
//     for (int i = 0; i < map_size; ++i) {
//         char ch;
//         int f;
//         input.get(ch);
//         input >> f;
//         input.get();
//         freq[ch] = f;
//     }

//     auto root = build_huffman_tree(freq);
//     BitReader reader(input);
//     std::ofstream output(output_path, std::ios::binary);

//     auto node = root;
//     bool bit;
//     while (reader.read_bit(bit)) {
//         node = bit ? node->right : node->left;
//         if (node->is_leaf()) {
//             output.put(node->ch);
//             node = root;
//         }
//     }
// }

#include "decompressor.hpp"
#include "huffman.hpp"
#include "bitstream.hpp"
#include <fstream>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <stdexcept>

template<typename T>
static T read_u(std::istream& is){
    T v = 0;
    for (size_t i = 0; i < sizeof(T); ++i){
        int b = is.get();
        if (b == EOF) throw std::runtime_error("unexpected EOF");
        v |= uint64_t(uint8_t(b)) << (8*i);
    }
    return v;
}

void decompress_file(const std::string& in_path,
                     const std::string& out_path)
{
    std::ifstream in(in_path, std::ios::binary);
    std::ofstream out(out_path, std::ios::binary);
    
    uint64_t original = read_u<uint64_t>(in);
    uint16_t m        = read_u<uint16_t>(in);

    std::unordered_map<char,int> freq;
    for (uint16_t i = 0; i < m; ++i){
        char c  = in.get();
        freq[c] = read_u<uint32_t>(in);
    }
    
    auto root = build_huffman_tree(freq);

    if (root->is_leaf()){
        std::vector<char> buf(4096, root->ch);
        uint64_t remaining = original;
        while (remaining){
            size_t chunk = std::min<uint64_t>(remaining, buf.size());
            out.write(buf.data(), chunk);
            remaining -= chunk;
        }
        return;
    }

    BitReader br(in);
    uint64_t emitted = 0;
    auto      node   = root;
    bool      bit;

    while (emitted < original && br.read_bit(bit)){
        node = bit ? node->right : node->left;
        if (node->is_leaf()){
            out.put(node->ch);
            node = root;
            ++emitted;
        }
    }
    if (emitted != original)
        throw std::runtime_error("truncated or corrupt bit-stream");
}
