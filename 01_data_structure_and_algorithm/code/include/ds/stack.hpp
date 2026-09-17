// stack.hpp —— 顺序栈 + 最小栈（对应 doc：01_基础数据结构/03_栈）
//
// 概念：后进先出（LIFO），只在栈顶做 push/pop。经典场景：函数调用栈、括号匹配。
// 额外实现"最小栈"：在常数时间(均摊)内取当前最小值 —— 用两个栈，辅栈只压"新的更小值"。
#pragma once

#include <stdexcept>

namespace ds {

template <typename T>
class Stack {
public:
    void push(const T& v) { data_[top_] = v; ++top_; }
    void pop() {
        if (empty()) throw std::underflow_error("栈为空");
        --top_;
    }
    T top() const {
        if (empty()) throw std::underflow_error("栈为空");
        return data_[top_ - 1];
    }
    bool empty() const { return top_ == 0; }
    int size() const { return top_; }
    void clear() { top_ = 0; }

private:
    static const int MAX = 1024;
    T data_[MAX];
    int top_ = 0;
};

// 最小栈：额外维护一个只压"新最小"的辅助栈
template <typename T>
class MinStack {
public:
    void push(const T& v) {
        data_[top_] = v; ++top_;
        if (minTop_ == 0 || v <= data_[minTop_ - 1]) { mins_[minTop_] = v; ++minTop_; }
    }
    void pop() {
        if (empty()) throw std::underflow_error("栈为空");
        if (top() == mins_[minTop_ - 1]) --minTop_;  // 弹出去的是当前最小
        --top_;
    }
    T top() const { return data_[top_ - 1]; }
    T min() const {
        if (minTop_ == 0) throw std::underflow_error("栈为空");
        return mins_[minTop_ - 1];
    }
    bool empty() const { return top_ == 0; }

private:
    static const int MAX = 1024;
    T data_[MAX], mins_[MAX];
    int top_ = 0, minTop_ = 0;
};

} // namespace ds