// hash_map.hpp —— 哈希表（链地址法）（对应 doc：01_基础数据结构/02_哈希表）
//
// 概念：用哈希函数把键映射到桶下标，期望下 O(1) 找出/插入/删除。
// 冲突处理用"链地址法"：每个桶挂一个链表，多个同桶键串在链上。
// 装填因子超过阈值时 rehash（翻倍桶数、把旧键重新散列）以维持 O(1)。
#pragma once

#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ds {

template <typename K, typename V,
          typename Hash = std::hash<K>>
class HashMap {
    struct Entry { K key; V val; Entry* next = nullptr; Entry(K k, V v) : key(k), val(v) {} };

public:
    explicit HashMap(size_t bucketCount = 16, float loadFactor = 0.75)
        : buckets_(bucketCount), loadFactor_(loadFactor) {}

    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;
    ~HashMap() { clear(); }

    void put(const K& key, const V& val) {
        size_t idx = index(key);
        // 已存在则覆盖
        for (Entry* e = buckets_[idx]; e; e = e->next)
            if (e->key == key) { e->val = val; return; }
        // 不存在则头插
        Entry* e = new Entry(key, val);
        e->next = buckets_[idx];
        buckets_[idx] = e;
        ++count_;
        if ((float)count_ > loadFactor_ * buckets_.size()) rehash();
    }

    bool contains(const K& key) const {
        for (Entry* e = buckets_[index(key)]; e; e = e->next)
            if (e->key == key) return true;
        return false;
    }

    V get(const K& key) const {
        for (Entry* e = buckets_[index(key)]; e; e = e->next)
            if (e->key == key) return e->val;
        throw std::out_of_range("HashMap: 键不存在");
    }

    V& at(const K& key) {
        for (Entry* e = buckets_[index(key)]; e; e = e->next)
            if (e->key == key) return e->val;
        throw std::out_of_range("HashMap: 键不存在");
    }

    // 有则返回 true，没有则插入默认值 false
    bool getOrPut(const K& key, const V& def, V& out) {
        for (Entry* e = buckets_[index(key)]; e; e = e->next)
            if (e->key == key) { out = e->val; return true; }
        put(key, def); out = def; return false;
    }

    bool remove(const K& key) {
        size_t idx = index(key);
        Entry* prev = nullptr;
        for (Entry* e = buckets_[idx]; e; prev = e, e = e->next)
            if (e->key == key) {
                if (prev) prev->next = e->next; else buckets_[idx] = e->next;
                delete e; --count_; return true;
            }
        return false;
    }

    size_t size() const { return count_; }
    bool empty() const { return count_ == 0; }

    void clear() {
        for (auto& head : buckets_) {
            while (head) { Entry* d = head; head = head->next; delete d; }
        }
        count_ = 0;
    }

private:
    std::vector<Entry*> buckets_;
    float loadFactor_;
    size_t count_ = 0;

    size_t index(const K& key) const { return Hash{}(key) % buckets_.size(); }

    // rehash：翻倍并重新分桶
    void rehash() {
        std::vector<Entry*> old = std::move(buckets_);
        buckets_.assign(old.size() * 2, nullptr);
        for (auto* head : old) {
            for (Entry* e = head; e;) {
                Entry* nx = e->next;
                size_t idx = Hash{}(e->key) % buckets_.size();
                e->next = buckets_[idx]; buckets_[idx] = e;
                e = nx;
            }
        }
    }
};

} // namespace ds