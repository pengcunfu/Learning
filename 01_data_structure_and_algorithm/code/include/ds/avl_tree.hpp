// avl_tree.hpp —— 平衡二叉树 AVL（对应 doc：02_树结构/03_平衡二叉树AVL）
//
// 动机：BST 最坏退化成链表。AVL 给每个节点记"平衡因子"(左右子树高度差)，
// 保持其绝对值 ≤1，从而保证 O(log n)。插入/删除后沿路径自底向上重新平衡。
// 四种旋转：
//   LL（插到左孩子的左）→ 右旋；RR（右孩子的右）→ 左旋
//   LR（左孩子的右）→ 先左旋再右旋；RL（右孩子的左）→ 先右旋再左旋
#pragma once

#include <algorithm>
#include <stdexcept>

namespace ds {

template <typename Key>
class AVLTree {
    struct Node {
        Key k;
        Node* l = nullptr; Node* r = nullptr;
        int h = 1; // 以该节点为根的子树高度
        explicit Node(const Key& key) : k(key) {}
    };
public:
    AVLTree() = default;
    ~AVLTree() { destroy(root_); }
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    void insert(const Key& k) { root_ = insert_(root_, k); }
    void remove(const Key& k) { root_ = remove_(root_, k); }
    bool contains(const Key& k) const {
        for (Node* n = root_; n;) {
            if (k < n->k) n = n->l;
            else if (n->k < k) n = n->r;
            else return true;
        }
        return false;
    }
    size_t size() const { return count_; }

    int height() const { return root_ ? root_->h : 0; }

    // 验证整棵树是否保持 AVL 性质（递归）——测试里用它确认"插入后仍平衡"
    bool check() const { return check_(root_) >= 0; }

    Node* root() const { return root_; }

private:
    Node* root_ = nullptr;
    size_t count_ = 0;

    static int height_(Node* n) { return n ? n->h : 0; }
    static int factor_(Node* n) { return n ? height_(n->l) - height_(n->r) : 0; }
    static void updateH_(Node* n) { n->h = std::max(height_(n->l), height_(n->r)) + 1; }

    // 左偏->右旋；右偏->左旋
    static Node* rotateR_(Node* y) { Node* x = y->l; y->l = x->r; x->r = y; updateH_(y); updateH_(x); return x; }
    static Node* rotateL_(Node* x) { Node* y = x->r; x->r = y->l; y->l = x; updateH_(x); updateH_(y); return y; }

    Node* balance_(Node* n) {
        updateH_(n);
        if (factor_(n) > 1) {
            if (factor_(n->l) < 0) n->l = rotateL_(n->l); // LR
            return rotateR_(n);                            // 结束为 LL
        }
        if (factor_(n) < -1) {
            if (factor_(n->r) > 0) n->r = rotateR_(n->r); // RL
            return rotateL_(n);                            // 结束为 RR
        }
        return n;
    }

    Node* insert_(Node* n, const Key& k) {
        if (!n) { ++count_; return new Node(k); }
        if (k < n->k) n->l = insert_(n->l, k);
        else if (n->k < k) n->r = insert_(n->r, k);
        else return n; // 已存在
        return balance_(n);
    }

    Node* minNode_(Node* n) const { while (n->l) n = n->l; return n; }

    Node* remove_(Node* n, const Key& k) {
        if (!n) return nullptr;
        if (k < n->k) n->l = remove_(n->l, k);
        else if (n->k < k) n->r = remove_(n->r, k);
        else {
            if (!n->l || !n->r) {
                Node* c = n->l ? n->l : n->r;
                delete n; --count_; return c;
            }
            Node* succ = minNode_(n->r);
            n->k = succ->k;
            n->r = remove_(n->r, succ->k);
        }
        return balance_(n);
    }

    // 递归验证：返回高度；若某处失去平衡返回 -1
    int check_(Node* n) const {
        if (!n) return 0;
        int l = check_(n->l), r = check_(n->r);
        if (l < 0 || r < 0) return -1;
        if (std::abs(l - r) > 1) return -1;
        return std::max(l, r) + 1;
    }

    void destroy(Node* n) { if (n) { destroy(n->l); destroy(n->r); delete n; } }
};

} // namespace ds