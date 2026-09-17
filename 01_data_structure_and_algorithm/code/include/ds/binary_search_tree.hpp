// binary_search_tree.hpp —— 二叉搜索树 BST（对应 doc：02_树结构/02_二叉搜索树BST）
//
// 性质：对任意节点，左子树所有值 < 该节点，右子树所有值 > 该节点。
// 于是查找/插入/删除在理想平衡时都是 O(log n)。最坏会退化成链表 O(n)，
// 这正是 AVL/红黑树存在的理由（后续文件）。
#pragma once

#include <cstddef>
#include <stdexcept>

namespace ds {

template <typename Key>
class BinarySearchTree {
public:
    struct Node {
        Key k;
        Node* left = nullptr;
        Node* right = nullptr;
        explicit Node(const Key& key) : k(key) {}
    };

    ~BinarySearchTree() { destroy(root_); }

    void insert(const Key& k) { root_ = insert_(root_, k); }

    // 查询：沿一条路径下钻，每次都能排除一半
    bool contains(const Key& k) const {
        Node* t = root_;
        while (t) {
            if (k < t->k) t = t->left;
            else if (t->k < k) t = t->right;
            else return true;
        }
        return false;
    }

    void remove(const Key& k) { root_ = remove_(root_, k); }

    size_t size() const { return count_; }
    bool empty() const { return count_ == 0; }

    Node* root() const { return root_; }

    Key min() const {
        if (!root_) throw std::underflow_error("树为空");
        Node* t = root_; while (t->left) t = t->left; return t->k;
    }
    Key max() const {
        if (!root_) throw std::underflow_error("树为空");
        Node* t = root_; while (t->right) t = t->right; return t->k;
    }

private:
    Node* root_ = nullptr;
    size_t count_ = 0;

    Node* insert_(Node* n, const Key& k) {
        if (!n) { ++count_; return new Node(k); }
        if (k < n->k) n->left = insert_(n->left, k);
        else if (n->k < k) n->right = insert_(n->right, k);
        return n; // 相等：已存在，不插入（保持集合语义）
    }

    Node* minNode_(Node* n) const { while (n->left) n = n->left; return n; }

    Node* remove_(Node* n, const Key& k) {
        if (!n) return nullptr; // 不存在：直接返回
        if (k < n->k) n->left = remove_(n->left, k);
        else if (n->k < k) n->right = remove_(n->right, k);
        else {
            // 找到了：用中序后继（右子树最小）顶替，度 0/1 直接摘除
            if (!n->left) { Node* r = n->right; delete n; --count_; return r; }
            if (!n->right) { Node* r = n->left; delete n; --count_; return r; }
            Node* succ = minNode_(n->right);
            n->k = succ->k;
            n->right = remove_(n->right, succ->k); // 删除那个后继
        }
        return n;
    }

    void destroy(Node* n) { if (n) { destroy(n->left); destroy(n->right); delete n; } }
};

} // namespace ds