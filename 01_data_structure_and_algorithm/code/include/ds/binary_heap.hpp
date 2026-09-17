// binary_heap.hpp —— 二叉堆（对应 doc：02_树结构/07_堆Heap）
//
// 概念：堆是一棵"完全二叉树"，用数组存储（下标 i 的父=(i-1)/2、左子=2i+1、右子=2i+2）。
// 最大堆满足：父 ≥ 子，于是堆顶永远是全局最大。
//   - push：尾部插入后上浮 siftUp（O(log n)）
//   - pop：把堆顶与末元素交换、删末，再下沉 siftDown（O(log n)）
//   - heapify：从最后一个有子节点处倒着下沉，O(n) 建堆
#pragma once

#include <stdexcept>
#include <vector>

namespace ds {

template <typename T>
class MaxHeap {
public:
    void push(const T& v) { a_.push_back(v); siftUp_(a_.size() - 1); }

    T top() const {
        if (a_.empty()) throw std::underflow_error("堆为空");
        return a_[0];
    }

    void pop() {
        if (a_.empty()) throw std::underflow_error("堆为空");
        std::swap(a_[0], a_.back()); a_.pop_back();
        if (!a_.empty()) siftDown_(0);
    }

    // 原地把任意给定数组调整成大顶堆：从最后一个内结点起倒着下沉
    static void heapify(std::vector<T>& arr) {
        for (long long i = (long long)(arr.size() / 2) - 1; i >= 0; --i)
            siftDown(arr, arr.size(), i);
    }

    size_t size() const { return a_.size(); }
    bool empty() const { return a_.empty(); }

    // 供堆排序使用：对 vector 的前 n 个元素在区间内下沉
    static void siftDown(std::vector<T>& arr, size_t n, size_t i) {
        for (;;) {
            size_t largest = i;
            size_t l = 2 * i + 1, r = 2 * i + 2;
            if (l < n && arr[l] > arr[largest]) largest = l;
            if (r < n && arr[r] > arr[largest]) largest = r;
            if (largest == i) break;
            std::swap(arr[i], arr[largest]); i = largest;
        }
    }

private:
    std::vector<T> a_;

    void siftUp_(size_t i) {
        while (i > 0 && a_[i] > a_[(i - 1) / 2]) {
            std::swap(a_[i], a_[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }
    void siftDown_(size_t i) { siftDown(a_, a_.size(), i); }
};

} // namespace ds