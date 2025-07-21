// #include "compressor.hpp"
// #include "huffman.hpp"
// #include "bitstream.hpp"
// #include <fstream>
// #include <unordered_map>

// void compress_file(const std::string& input_path, const std::string& output_path) {
//     std::ifstream input(input_path, std::ios::binary);
//     std::unordered_map<char, int> freq;

//     if (freq.size() == 1) {
//         char sym = freq.begin()->first;
//         std::ofstream out(output_path, std::ios::binary);
//         out.put(0xFF);                 // marker byte
//         out.put(sym);
//         uint64_t n = freq.begin()->second;
//         out.write(reinterpret_cast<char*>(&n), sizeof n);
//         return;
//     }

//     char ch;
//     while (input.get(ch)) freq[ch]++;

//     auto root = build_huffman_tree(freq);
//     std::unordered_map<char, std::string> codes;
//     generate_codes(root, "", codes);

//     input.clear(); input.seekg(0);
//     std::ofstream output(output_path, std::ios::binary);
//     output << freq.size() << "\n";
//     for (const auto& [c, f] : freq) output << c << ' ' << f << "\n";

//     BitWriter writer(output);
//     while (input.get(ch)) {
//         writer.write_bits(codes[ch]);
//     }
//     writer.flush();
// }

#include "compressor.hpp"
#include "huffman.hpp"
#include "bitstream.hpp"
#include <fstream>
#include <unordered_map>
#include <vector>
#include <cstdint>

// little-endian helper
template<typename T>
static void write_u(std::ostream& os, T v){
    for (size_t i=0;i<sizeof(T);++i){ os.put(v&0xFF); v>>=8; }
}

void compress_file(const std::string& input_path,
                   const std::string& output_path)
{
    std::ifstream in(input_path, std::ios::binary);
    std::vector<char> data((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());
    if (data.empty()) throw std::runtime_error("empty file");

    std::unordered_map<char,int> freq;
    for (char c : data) freq[c]++;

    std::ofstream out(output_path, std::ios::binary);
    
    if (freq.size() == 1) {
        out.put(0x01); 
        write_u<uint64_t>(out, data.size());
        out.put(freq.begin()->first);
        return;                                       
    }
    
    out.put(0x00);
    write_u<uint64_t>(out, data.size());
    write_u<uint16_t>(out, static_cast<uint16_t>(freq.size()));
    for (auto [c,f] : freq){ out.put(c); write_u<uint32_t>(out,f); }
    
    auto root = build_huffman_tree(freq);
    std::unordered_map<char,std::string> codes;
    generate_codes(root,"",codes);

    BitWriter bw(out);
    for (char c : data) bw.write_bits(codes[c]);
    bw.flush();
}
