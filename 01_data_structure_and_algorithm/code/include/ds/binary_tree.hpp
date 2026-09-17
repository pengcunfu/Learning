// binary_tree.hpp —— 二叉树遍历（对应 doc：02_树结构/01_树的基础理论）
//
// 概念：树是递归定义的层次结构；二叉树每个节点最多两个子节点。
// 本节实现四种遍历，体会"递归版很自然，迭代版要靠显式栈/队列"。
//   - 前序：根→左→右
//   - 中序：左→根→右
//   - 后序：左→右→根
//   - 层序：按层从上到下（BFS，用队列）
#pragma once

#include <queue>
#include <vector>

namespace ds {

template <typename T>
struct BinaryNode {
    T val;
    BinaryNode* left = nullptr;
    BinaryNode* right = nullptr;
    explicit BinaryNode(const T& v) : val(v) {}
};

namespace traversal {

template <typename T>
void preorder(BinaryNode<T>* n, std::vector<T>& out) {
    if (!n) return;
    out.push_back(n->val);
    preorder(n->left, out);
    preorder(n->right, out);
}

template <typename T>
void inorder(BinaryNode<T>* n, std::vector<T>& out) {
    if (!n) return;
    inorder(n->left, out);
    out.push_back(n->val);
    inorder(n->right, out);
}

template <typename T>
void postorder(BinaryNode<T>* n, std::vector<T>& out) {
    if (!n) return;
    postorder(n->left, out);
    postorder(n->right, out);
    out.push_back(n->val);
}

// 层序遍历（BFS）：用队列，逐层入队出队
template <typename T>
void levelorder(BinaryNode<T>* root, std::vector<T>& out) {
    if (!root) return;
    std::queue<BinaryNode<T>*> q;
    q.push(root);
    while (!q.empty()) {
        BinaryNode<T>* n = q.front(); q.pop();
        out.push_back(n->val);
        if (n->left) q.push(n->left);
        if (n->right) q.push(n->right);
    }
}

template <typename T>
int maxDepth(BinaryNode<T>* n) {
    if (!n) return 0;
    int l = maxDepth(n->left), r = maxDepth(n->right);
    return (l > r ? l : r) + 1;
}

} // namespace traversal
} // namespace ds