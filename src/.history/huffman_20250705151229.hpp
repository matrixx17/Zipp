#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

struct HuffmanNode {
    char ch;
    int freq;
    uint32_t 
    std::shared_ptr<HuffmanNode> left, right;
    HuffmanNode(char c, int f);
    HuffmanNode(std::shared_ptr<HuffmanNode> l, std::shared_ptr<HuffmanNode> r);
    bool is_leaf() const;
};

struct CompareNodes {
    bool operator()(const std::shared_ptr<HuffmanNode>& a, const std::shared_ptr<HuffmanNode>& b);
};

std::shared_ptr<HuffmanNode> build_huffman_tree(const std::unordered_map<char, int>& frequencies);
void generate_codes(const std::shared_ptr<HuffmanNode>& root, const std::string& code, std::unordered_map<char, std::string>& table);

