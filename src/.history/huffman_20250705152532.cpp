// #include "huffman.hpp"
// #include <queue>

// HuffmanNode::HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
// HuffmanNode::HuffmanNode(std::shared_ptr<HuffmanNode> l, std::shared_ptr<HuffmanNode> r)
//     : ch('\0'), freq(l->freq + r->freq), left(std::move(l)), right(std::move(r)) {}

// bool HuffmanNode::is_leaf() const {
//     return !left && !right;
// }

// bool CompareNodes::operator()(const std::shared_ptr<HuffmanNode>& a, const std::shared_ptr<HuffmanNode>& b) {
//     return a->freq > b->freq;
// }

// std::shared_ptr<HuffmanNode> build_huffman_tree(const std::unordered_map<char, int>& frequencies) {
//     std::priority_queue<std::shared_ptr<HuffmanNode>, std::vector<std::shared_ptr<HuffmanNode>>, CompareNodes> pq;
//     for (const auto& pair : frequencies) {
//         pq.push(std::make_shared<HuffmanNode>(pair.first, pair.second));
//     }
//     while (pq.size() > 1) {
//         auto left = pq.top(); pq.pop();
//         auto right = pq.top(); pq.pop();
//         pq.push(std::make_shared<HuffmanNode>(left, right));
//     }
//     return pq.top();
// }

// void generate_codes(const std::shared_ptr<HuffmanNode>& node, const std::string& code,
//                     std::unordered_map<char, std::string>& table) {
//     if (!node) return;
//     if (node->is_leaf()) {
//         table[node->ch] = code;
//     } else {
//         generate_codes(node->left, code + "0", table);
//         generate_codes(node->right, code + "1", table);
//     }
// }

#include "huffman.hpp"
#include <algorithm>
#include <queue>
#include <cassert>

static uint32_t g_rank_counter = 0;

HuffmanNode::HuffmanNode(char c, int f, uint32_t r)
    : ch(c), freq(f), rank(r), left(nullptr), right(nullptr) {}

HuffmanNode::HuffmanNode(std::shared_ptr<HuffmanNode> l,
                         std::shared_ptr<HuffmanNode> r)
    : ch('\0'),
      freq(l->freq + r->freq),
      rank(std::min(l->rank, r->rank)),
      left(std::move(l)),
      right(std::move(r)) {}

bool HuffmanNode::is_leaf() const { return !left && !right; }

// Comparator
bool CompareNodes::operator()(const std::shared_ptr<HuffmanNode>& a,
                              const std::shared_ptr<HuffmanNode>& b) const {
    if (a->freq != b->freq) return a->freq > b->freq;
    return a->rank > b->rank;
}

// Build tree
std::shared_ptr<HuffmanNode>
build_huffman_tree(const std::unordered_map<char,int>& frequencies)
{
    assert(!frequencies.empty());

    // sort so leaves get ranks in byte order ⇒ deterministic
    std::vector<std::pair<char,int>> items(frequencies.begin(),
                                           frequencies.end());
    std::sort(items.begin(), items.end(),
              [](const auto& a,const auto& b){ return a.first < b.first; });

    std::priority_queue<std::shared_ptr<HuffmanNode>,
                        std::vector<std::shared_ptr<HuffmanNode>>,
                        CompareNodes> pq;

    for (const auto& [c,f] : items)
        pq.push(std::make_shared<HuffmanNode>(c,f,g_rank_counter++));

    while (pq.size() > 1) {
        auto l = pq.top(); pq.pop();
        auto r = pq.top(); pq.pop();
        pq.push(std::make_shared<HuffmanNode>(l,r));
    }
    return pq.top();
}

// Generate codes
void generate_codes(const std::shared_ptr<HuffmanNode>& node,
                    const std::string& prefix,
                    std::unordered_map<char,std::string>& table)
{
    if (!node) return;
    if (node->is_leaf()) {
        table[node->ch] = prefix.empty() ? "0" : prefix;
        return;
    }
    generate_codes(node->left , prefix + "0", table);
    generate_codes(node->right, prefix + "1", table);
}
