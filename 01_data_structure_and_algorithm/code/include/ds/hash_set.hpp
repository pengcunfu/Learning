// hash_set.hpp —— 哈希集合（对应 doc：01_基础数据结构/07_集合）
//
// 概念：集合 = 不重复元素的容器，只关心"元素在不在"（成员检测/去重），
// 不关心顺序。底层用哈希表：把每个元素当键，值是占位。
#pragma once

#include <functional>
#include <vector>

namespace ds {

template <typename T, typename Hash = std::hash<T>>
class HashSet {
    struct Node { T val; Node* next = nullptr; Node(const T& v) : val(v) {} };
public:
    explicit HashSet(size_t bucketCount = 16, float loadFactor = 0.75)
        : buckets_(bucketCount), loadFactor_(loadFactor) {}

    HashSet(const HashSet&) = delete;
    HashSet& operator=(const HashSet&) = delete;
    ~HashSet() { clear(); }

    void insert(const T& v) {
        if (addCheck_(v)) return;                 // 已存在
        size_t idx = Hash{}(v) % buckets_.size();
        Node* n = new Node(v);
        n->next = buckets_[idx]; buckets_[idx] = n;
        ++count_;
        if ((float)count_ > loadFactor_ * buckets_.size()) rehash();
    }

    bool contains(const T& v) const { return addCheck_(v); }

    bool remove(const T& v) {
        size_t idx = Hash{}(v) % buckets_.size();
        Node* prev = nullptr;
        for (Node* n = buckets_[idx]; n; prev = n, n = n->next)
            if (n->val == v) {
                if (prev) prev->next = n->next; else buckets_[idx] = n->next;
                delete n; --count_; return true;
            }
        return false;
    }

    size_t size() const { return count_; }
    bool empty() const { return count_ == 0; }
    void clear() {
        for (auto& h : buckets_) while (h) { Node* d = h; h = h->next; delete d; }
        count_ = 0;
    }

private:
    std::vector<Node*> buckets_;
    float loadFactor_;
    size_t count_ = 0;

    bool addCheck_(const T& v) const {
        for (Node* n = buckets_[Hash{}(v) % buckets_.size()]; n; n = n->next)
            if (n->val == v) return true;
        return false;
    }
    void rehash() {
        std::vector<Node*> old = std::move(buckets_);
        buckets_.assign(old.size() * 2, nullptr);
        for (auto* h : old)
            for (Node* n = h; n;) {
                Node* nx = n->next;
                size_t idx = Hash{}(n->val) % buckets_.size();
                n->next = buckets_[idx]; buckets_[idx] = n;
                n = nx;
            }
    }
};

} // namespace ds