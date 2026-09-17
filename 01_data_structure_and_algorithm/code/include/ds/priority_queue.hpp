// priority_queue.hpp —— 优先队列（对应 doc：02_树结构/07_堆Heap 的应用）
//
// 概念：优先队列 = 总是取"当前最大（或最小）"元素的队列，底层正是堆。
// 典型应用：Dijkstra 的堆优化、任务调度、Top-K。这里就是给 MaxHeap 一个别名概念：
// 一个能用 push / top / pop 操作的抽象，堆是实现。min 版只需换比较方向。
#pragma once

#include <functional>
#include <vector>

namespace ds {

// 通用优先队列：Compare 默认按 > 构成"大顶堆"（队首最大），
// 传入 std::greater<> 则变"小顶堆"（队首最小）——供 Top-K、Dijkstra 使用。
template <typename T, typename Compare = std::less<T>>
class PriorityQueue {
    std::vector<T> a_;
public:
    void push(const T& v) { a_.push_back(v); up_(a_.size() - 1); }
    T top() const { return a_[0]; }
    void pop() { std::swap(a_[0], a_.back()); a_.pop_back(); if (!a_.empty()) down_(0); }
    bool empty() const { return a_.empty(); }
    size_t size() const { return a_.size(); }

private:
    Compare cmp_;
    // "higher"：x 是否应排在 y 之上（堆顶/优先）。cmp 默认 std::less→x>=y 则 x 优先(大顶)；
    // 传入 std::greater→x<=y 则 x 优先(小顶)。即 top() = 不被 cmp 比下去的那个。
    bool higher_(const T& x, const T& y) const { return !cmp_(x, y); }
    void up_(size_t i) {
        while (i > 0 && higher_(a_[i], a_[(i - 1) / 2])) { std::swap(a_[i], a_[(i - 1) / 2]); i = (i - 1) / 2; }
    }
    void down_(size_t i) {
        size_t n = a_.size();
        for (;;) {
            size_t best = i, l = 2 * i + 1, r = 2 * i + 2;
            if (l < n && higher_(a_[l], a_[best])) best = l;
            if (r < n && higher_(a_[r], a_[best])) best = r;
            if (best == i) break;
            std::swap(a_[i], a_[best]); i = best;
        }
    }
};

} // namespace ds