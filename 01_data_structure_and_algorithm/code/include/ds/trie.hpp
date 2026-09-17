// trie.hpp —— 字典树 Trie（对应 doc：02_树结构/08_Trie树字典树）
//
// 概念：用多叉树按"字符前缀"存字符串，根到某个标记节点的路径即一个单词。
// 优点：支持找最长公共前缀、自动补全；按前缀查找只花 O(|前缀|)。
// 缺点：内存开销大（每个节点 26/字符集 个孩子）。适合大量同前缀字符串。
#pragma once

#include <array>
#include <string>
#include <vector>

namespace ds {

constexpr int ALPHABET = 26;

class Trie {
    struct Node {
        std::array<Node*, ALPHABET> next{nullptr};
        bool isWord = false;
    };
public:
    Trie() = default;
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
    ~Trie() { release(&root_); }

    void insert(const std::string& s) {
        Node* cur = &root_;
        for (char c : s) {
            int i = c - 'a';
            if (!cur->next[i]) cur->next[i] = new Node();
            cur = cur->next[i];
        }
        if (!cur->isWord) { cur->isWord = true; ++total_; }
    }

    // 是否完整包含该单词
    bool contains(const std::string& s) const {
        Node* cur = findPrefix(s);
        return cur != nullptr && cur->isWord;
    }

    // 是否存在以该前缀开头的单词
    bool hasPrefix(const std::string& s) const { return findPrefix(s) != nullptr; }

    size_t size() const { return total_; }

    // 自动补全：返回所有以 prefix 开头的单词，最多 limit 个（按字母序收集）
    std::vector<std::string> autocomplete(const std::string& prefix, int limit = 100) const {
        std::vector<std::string> out;
        Node* cur = findPrefix(prefix);
        if (cur) collect_(cur, prefix, out, limit);
        return out;
    }

private:
    Node root_;                 // 非指针的根哨兵：孩子全为 nullptr、isWord=false
    size_t total_ = 0;

    Node* findPrefix(const std::string& s) const {
        Node* cur = const_cast<Node*>(&root_);
        for (char c : s) {
            int i = c - 'a';
            if (!cur->next[i]) return nullptr;
            cur = cur->next[i];
        }
        return cur;
    }

    void collect_(Node* n, std::string prefix, std::vector<std::string>& out, int limit) const {
        if (static_cast<int>(out.size()) >= limit) return;
        if (n->isWord) out.push_back(prefix);
        for (int i = 0; i < ALPHABET; ++i)
            if (n->next[i]) collect_(n->next[i], prefix + char('a' + i), out, limit);
    }

    void release(Node* n) {
        for (auto* c : n->next) if (c) release(c);
        if (n != &root_) delete n;   // 根哨兵是成员对象，不 delete
    }
};

} // namespace ds