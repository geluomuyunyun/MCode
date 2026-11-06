// Trie.h - 字典树（前缀树）。
//
// 每条边代表一个字符，从根到某结点的路径就是一个前缀。
// 查找/插入的时间只与单词长度有关，与词典大小无关：O(L)。
// 每个结点记录 pass（有多少个单词经过这里），以支持前缀计数和删除时剪枝。
// 孩子用 std::map 存储，遍历时天然按字典序输出。
#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Trie {
public:
    bool insert(const std::string& word);          // 单词已存在返回 false
    bool contains(const std::string& word) const;
    bool starts_with(const std::string& prefix) const;
    bool erase(const std::string& word);           // 删除后剪掉无用分支
    std::size_t count_prefix(const std::string& prefix) const;
    std::vector<std::string> words_with_prefix(const std::string& prefix) const; // 字典序
    std::string longest_common_prefix() const;     // 所有单词的最长公共前缀
    std::string longest_prefix_of(const std::string& text) const; // 词典中是 text 前缀的最长单词
    std::size_t size() const { return words_; }

private:
    struct Node {
        std::map<char, std::unique_ptr<Node>> next;
        std::size_t pass = 0;
        bool end = false;
    };

    const Node* find_node(const std::string& s) const;
    static void collect(const Node* n, std::string& path, std::vector<std::string>& out);

    Node root_;
    std::size_t words_ = 0;
};
