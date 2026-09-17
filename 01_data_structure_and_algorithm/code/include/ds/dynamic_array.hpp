// dynamic_array.hpp —— 动态数组（对应 doc：01_基础数据结构/01_数组）
//
// 概念：数组是连续内存、按下标 O(1) 随机访问的线性结构。
// 动态数组在其上加了"自动扩容"：装满后按需搬进更大的连续区。
// 教学点：为什么下标访问快？因为 base + index*size 一次寻址。
#pragma once

#include <cstddef>
#include <stdexcept>

namespace ds {

template <typename T>
class DynamicArray {
public:
    DynamicArray(size_t cap = 4) : cap_(cap), data_(new T[cap]) {}
    ~DynamicArray() { delete[] data_; }

    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    size_t size() const { return size_; }
    size_t capacity() const { return cap_; }
    bool empty() const { return size_ == 0; }

    T& operator[](size_t i) { return data_[i]; }
    const T& operator[](size_t i) const { return data_[i]; }

    T& at(size_t i) {
        if (i >= size_) throw std::out_of_range("DynamicArray::at 越界");
        return data_[i];
    }

    T& front() { return data_[0]; }
    T& back() { return data_[size_ - 1]; }

    void push_back(const T& v) {
        if (size_ == cap_) grow();
        data_[size_++] = v;
    }

    void pop_back() {
        if (size_ == 0) throw std::underflow_error("DynamicArray 为空");
        --size_;
    }

    // 在 i 处插入（i<=size），把 i 及之后元素后移
    void insert(size_t i, const T& v) {
        if (i > size_) throw std::out_of_range("insert 越界");
        if (size_ == cap_) grow();
        for (size_t k = size_; k > i; --k) data_[k] = data_[k - 1];
        data_[i] = v;
        ++size_;
    }

    // 删除 i 处元素，后续元素前移
    void erase(size_t i) {
        if (i >= size_) throw std::out_of_range("erase 越界");
        for (size_t k = i; k + 1 < size_; ++k) data_[k] = data_[k + 1];
        --size_;
    }

    int find(const T& v) const {
        for (size_t i = 0; i < size_; ++i)
            if (data_[i] == v) return static_cast<int>(i);
        return -1;
    }

    void clear() { size_ = 0; }

private:
    size_t size_ = 0, cap_ = 0;
    T* data_ = nullptr;

    // 扩容：搬进 2 倍大的连续区 —— O(n)，但均摊下来每次 push_back 仍是 O(1)
    void grow() {
        size_t newCap = cap_ ? cap_ * 2 : 4;
        T* nd = new T[newCap];
        for (size_t i = 0; i < size_; ++i) nd[i] = data_[i];
        delete[] data_;
        data_ = nd;
        cap_ = newCap;
    }
};

} // namespace ds