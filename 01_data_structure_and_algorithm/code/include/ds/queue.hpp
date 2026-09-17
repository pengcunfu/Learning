// queue.hpp —— 循环队列 + 链式队列（对应 doc：01_基础数据结构/04_队列）
//
// 概念：先进先出（FIFO），队尾 enqueue、队头 dequeue。BFS 依赖队列。
// 循环队列用一个环形下标（front_ 与 back_），避免"出队后前面空、后面满"的假满，
// 做到数组 O(1) 复用空间。
#pragma once

#include <cstddef>
#include <stdexcept>

namespace ds {

// 环形数组队列：front_ 指向队头，back_ 指向下一个写入位；空/满用"留一格"区分。
template <typename T>
class CircularQueue {
public:
    explicit CircularQueue(size_t cap = 8) : cap_(cap), data_(new T[cap]) {}
    ~CircularQueue() { delete[] data_; }

    void push(const T& v) {
        if (full()) grow();
        data_[back_] = v;
        back_ = (back_ + 1) % cap_;
        ++size_;
    }

    void pop() {
        if (empty()) throw std::underflow_error("队列为空");
        front_ = (front_ + 1) % cap_;
        --size_;
    }

    T front() const {
        if (empty()) throw std::underflow_error("队列为空");
        return data_[front_];
    }

    T back() const {
        if (empty()) throw std::underflow_error("队列为空");
        return data_[(back_ + cap_ - 1) % cap_];
    }

    bool empty() const { return size_ == 0; }
    bool full() const { return size_ == cap_; }
    size_t size() const { return size_; }

private:
    size_t cap_ = 0, size_ = 0, front_ = 0, back_ = 0;
    T* data_ = nullptr;

    void grow() {
        size_t nc = cap_ * 2;
        T* nd = new T[nc];
        for (size_t k = 0; k < size_; ++k) nd[k] = data_[(front_ + k) % cap_];
        delete[] data_;
        data_ = nd; cap_ = nc; front_ = 0; back_ = size_;
    }
};

// 链式队列：无容量上限，队头出、队尾进
template <typename T>
class LinkedQueue {
    struct Node { T v; Node* next = nullptr; Node(const T& x) : v(x) {} };
public:
    ~LinkedQueue() { while (head_) { Node* d = head_; head_ = head_->next; delete d; } }

    void push(const T& v) {
        Node* n = new Node(v);
        if (tail_) tail_->next = n; else head_ = n;
        tail_ = n; ++size_;
    }
    void pop() {
        if (empty()) throw std::underflow_error("队列为空");
        Node* d = head_; head_ = head_->next; if (!head_) tail_ = nullptr; delete d; --size_;
    }
    T front() const { return head_->v; }
    bool empty() const { return head_ == nullptr; }
    size_t size() const { return size_; }

private:
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    size_t size_ = 0;
};

} // namespace ds