// red_black_tree.hpp —— 红黑树（对应 doc：02_树结构/04_红黑树）
//
// 动机：AVL 太"较真"（频繁旋转），红黑树放宽条件仍保证 O(log n)，
// 且插入/删除旋转次数更少，是 std::map/set 的常用底层。
//
// 五条性质（保证最坏 O(log n)）：
//   1. 节点非红即黑；2. 根黑；3. 叶子(NIL)黑；4. 红节点的孩子必黑（不相邻双红）
//   5. 任一节点到其所有 NIL 叶子路径上的黑节点数相同。
// 插入/删除后用"变色 + 旋转"修复，把失衡逐层向上推。
#pragma once

#include <stdexcept>

namespace ds {

template <typename Key>
class RedBlackTree {
    enum Color { RED, BLACK };
    struct Node {
        Key k; Color c = RED;
        Node* p = nullptr; Node* l = nullptr; Node* r = nullptr;
        explicit Node(const Key& key) : k(key) {}
    };
    // NIL 哨兵：所有空指针都指向它，统一当作黑色叶子。
// 关键：NIL 的孩子必须指向自身（而非 nullptr），否则删除修复里读"兄弟的孩子"
// 会解空指针。哨兵只初始化一次（不再被反复重置颜色）。
    static Node* NIL() {
        static Node* s = [] {
            Node* n = new Node(Key{});
            n->c = BLACK;
            n->l = n->r = n->p = n;   // 自环
            return n;
        }();
        return s;
    }
    // 注：Key{} 需可默认构造；若禁用可自行改为专用哨兵。

public:
    explicit RedBlackTree() { root_ = NIL(); }
    ~RedBlackTree() { destroy_(root_); }

    bool contains(const Key& k) const {
        Node* x = root_;
        while (x != NIL()) {
            if (k < x->k) x = x->l;
            else if (x->k < k) x = x->r;
            else return true;
        }
        return false;
    }
    void insert(const Key& k) {
        Node* z = new Node(k); z->l = z->r = NIL();
        Node* y = NIL(); Node* x = root_;
        while (x != NIL()) { y = x; x = (k < x->k) ? x->l : x->r; }
        z->p = y;
        if (y == NIL()) root_ = z;
        else if (k < y->k) y->l = z; else y->r = z;
        insertFixup_(z);
        ++count_;
        NIL()->c = BLACK;   // 共享哨兵颜色在 key 修复中可能被临时改动，复位防止漂移
    }
    void remove(const Key& k) {
        Node* z = search_(root_, k);
        if (z != NIL()) { removeNode_(z); --count_; }
        NIL()->c = BLACK;
    }
    size_t size() const { return count_; }
    // 校验五条性质（测试用）：根黑、无相邻双红、黑高一致
    bool check() const {
        if (root_ == NIL()) return true;
        if (root_->c != BLACK) return false;             // 规则2：根黑
        return checkSubtree_(root_) >= 0;
    }
private:
    Node* root_ = nullptr;
    size_t count_ = 0;

    // -- insert 修复 --
    void insertFixup_(Node* z) {
        while (z->p && z->p->c == RED && z->p != NIL()) {
            if (z->p == z->p->p->l) {          // 父在祖父左边
                Node* u = z->p->p->r;          // 叔叔
                if (u->c == RED) {             // 情形1：叔叔红 → 变色上推
                    z->p->c = BLACK; u->c = BLACK; z->p->p->c = RED; z = z->p->p;
                } else {
                    if (z == z->p->r) { z = z->p; rotateL_(z); }       // 情形2 → 变情形3
                    z->p->c = BLACK; z->p->p->c = RED; rotateR_(z->p->p); // 情形3：右旋+变色
                }
            } else {                           // 父在祖父右边（镜像）
                Node* u = z->p->p->l;
                if (u->c == RED) { z->p->c = BLACK; u->c = BLACK; z->p->p->c = RED; z = z->p->p; }
                else {
                    if (z == z->p->l) { z = z->p; rotateR_(z); }
                    z->p->c = BLACK; z->p->p->c = RED; rotateL_(z->p->p);
                }
            }
        }
        root_->c = BLACK;
    }

    // -- delete 修复 --
    void removeNode_(Node* z) {
        Node* y = z; Color yOC = y->c;
        Node* x;
        if (z->l == NIL()) { x = z->r; transplant_(z, z->r); }
        else if (z->r == NIL()) { x = z->l; transplant_(z, z->l); }
        else {
            y = treeMin_(z->r); yOC = y->c; x = y->r;
            if (y->p == z) { x->p = y; }
            else { transplant_(y, y->r); y->r = z->r; y->r->p = y; }
            transplant_(z, y); y->l = z->l; y->l->p = y; y->c = z->c;
        }
        delete z;
        if (yOC == BLACK) deleteFixup_(x);
    }
    void transplant_(Node* u, Node* v) {   // 用 v 子树替换 u
        if (u->p == NIL()) root_ = v;
        else if (u == u->p->l) u->p->l = v; else u->p->r = v;
        v->p = u->p;
    }
    void deleteFixup_(Node* x) {
        while (x != root_ && x->c == BLACK) {
            if (x == x->p->l) {
                Node* w = x->p->r;                    // 兄弟
                if (w->c == RED) { w->c = BLACK; x->p->c = RED; rotateL_(x->p); w = x->p->r; }
                if (w->l->c == BLACK && w->r->c == BLACK) {
                    w->c = RED;                        // 教科书情形2：w 变红后上移
                    x = x->p;
                }
                else {
                    if (w->r->c == BLACK) { w->l->c = BLACK; w->c = RED; rotateR_(w); w = x->p->r; }
                    w->c = x->p->c; x->p->c = BLACK; w->r->c = BLACK; rotateL_(x->p); x = root_;
                }
            } else {                                    // 镜像
                Node* w = x->p->l;
                if (w->c == RED) { w->c = BLACK; x->p->c = RED; rotateR_(x->p); w = x->p->l; }
                if (w->r->c == BLACK && w->l->c == BLACK) {
                    w->c = RED;
                    x = x->p;
                }
                else {
                    if (w->l->c == BLACK) { w->r->c = BLACK; w->c = RED; rotateL_(w); w = x->p->l; }
                    w->c = x->p->c; x->p->c = BLACK; w->l->c = BLACK; rotateR_(x->p); x = root_;
                }
            }
        }
        x->c = BLACK;   // NIL 已是黑色，赋值无妨
    }

    void rotateL_(Node* x) {
        Node* y = x->r; x->r = y->l; if (y->l != NIL()) y->l->p = x; y->p = x->p;
        if (x->p == NIL()) root_ = y;
        else if (x == x->p->l) x->p->l = y; else x->p->r = y;
        y->l = x; x->p = y;
    }
    void rotateR_(Node* x) {
        Node* y = x->l; x->l = y->r; if (y->r != NIL()) y->r->p = x; y->p = x->p;
        if (x->p == NIL()) root_ = y;
        else if (x == x->p->r) x->p->r = y; else x->p->l = y;
        y->r = x; x->p = y;
    }

    Node* search_(Node* n, const Key& k) const {
        while (n != NIL() && !(n->k == k)) n = (k < n->k) ? n->l : n->r;
        return n;
    }
    Node* treeMin_(Node* n) const { while (n->l != NIL()) n = n->l; return n; }

    // 递归：返回 node 到叶子的黑高；遇到"相邻双红"或"左右黑高不一致"返回 -1
    int checkSubtree_(Node* n) const {
        if (n == NIL()) return 1;                     // NIL 算作黑叶子
        if (n->c == RED && (n->l->c == RED || n->r->c == RED)) return -1; // 规则4
        int lb = checkSubtree_(n->l);
        int rb = checkSubtree_(n->r);
        if (lb < 0 || rb < 0 || lb != rb) return -1;  // 规则5
        return lb + (n->c == BLACK ? 1 : 0);
    }
    void destroy_(Node* n) {
        if (n == NIL() || !n) return;                 // NIL 是静态对象，不释放
        destroy_(n->l); destroy_(n->r); delete n;
    }
};

} // namespace ds