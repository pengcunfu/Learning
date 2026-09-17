// linked_list.hpp —— 单链表（对应 doc：01_基础数据结构/05_链表）
//
// 概念：链表节点在内存中不连续，用指针"串"起来；因此插入/删除 O(1)、
// 随机访问 O(n)（与数组互补）。本文件还实现链表经典的三个手法：
//   - 反转链表（迭代）
//   - 快慢指针找中点 / 判环（Floyd 快慢指针）
#pragma once

#include <cstddef>
#include <stdexcept>

namespace ds {

template <typename T>
class LinkedList {
public:
    struct Node {
        T val;
        Node* next = nullptr;
        Node(const T& v) : val(v) {}
    };

    LinkedList() = default;
    ~LinkedList() { clear(); }
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void push_front(const T& v) {
        Node* n = new Node(v);
        n->next = head_;
        head_ = n;
        ++size_;
    }

    void push_back(const T& v) {
        Node* n = new Node(v);
        if (!head_) { head_ = n; }
        else { Node* t = head_; while (t->next) t = t->next; t->next = n; }
        ++size_;
    }

    void pop_front() {
        if (!head_ || size_ == 0) throw std::underflow_error("链表为空");
        Node* d = head_; head_ = head_->next; delete d; --size_;
    }

    T front() const {
        if (!head_) throw std::underflow_error("链表为空");
        return head_->val;
    }

    T back() const {
        if (!head_) throw std::underflow_error("链表为空");
        Node* t = head_; while (t->next) t = t->next;
        return t->val;
    }

    bool contains(const T& v) const {
        for (Node* t = head_; t; t = t->next) if (t->val == v) return true;
        return false;
    }

    // 删除第一个值为 v 的节点，返回是否删除成功
    bool remove(const T& v) {
        Node* prev = nullptr;
        for (Node* t = head_; t; prev = t, t = t->next) {
            if (t->val == v) {
                if (prev) prev->next = t->next; else head_ = t->next;
                delete t; --size_; return true;
            }
        }
        return false;
    }

    // 迭代反转链表，返回新头
    Node* reverse() {
        Node* prev = nullptr;
        Node* cur = head_;
        while (cur) { Node* nxt = cur->next; cur->next = prev; prev = cur; cur = nxt; }
        head_ = prev;
        return head_;
    }

    // 快慢指针：慢指针每次走1、快指针每次走2。有环则必然相遇。
    bool has_cycle() const {
        if (!head_ || !head_->next) return false;
        Node* slow = head_; Node* fast = head_;
        while (fast && fast->next) {
            slow = slow->next; fast = fast->next->next;
            if (slow == fast) return true;
        }
        return false;
    }

    // 快慢指针找中间节点（节点数为偶数时偏右）
    T middle() const {
        if (!head_) throw std::underflow_error("链表为空");
        Node* slow = head_; Node* fast = head_;
        while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
        return slow->val;
    }

    Node* begin() const { return head_; }
    Node* head() const { return head_; }

    void clear() {
        while (head_) { Node* d = head_; head_ = head_->next; delete d; }
        size_ = 0;
    }

private:
    Node* head_ = nullptr;
    size_t size_ = 0;
};

} // namespace ds