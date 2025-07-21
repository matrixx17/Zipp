#include "compressor.hpp"
#include "huffman.hpp"
#include "bitstream.hpp"
#include <fstream>
#include <unordered_map>

void compress_file(const std::string& input_path, const std::string& output_path) {
    std::ifstream input(input_path, std::ios::binary);
    std::unordered_map<char, int> freq;
    char ch;
    while (input.get(ch)) freq[ch]++;

    auto root = build_huffman_tree(freq);
    std::unordered_map<char, std::string> codes;
    generate_codes(root, "", codes);

    input.clear(); input.seekg(0);
    std::ofstream output(output_path, std::ios::binary);
    output << freq.size() << "\n";
    for (const auto& [c, f] : freq) output << c << ' ' << f << "\n";

    BitWriter writer(output);
    while (input.get(ch)) {
        writer.write_bits(codes[ch]);
    }
    writer.flush();
}