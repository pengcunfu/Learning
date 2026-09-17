// binary_search.hpp —— 折半查找（对应 doc：04_算法设计/03_查找算法/折半查找）
//
// 前置：数组必须有序。每次取中点点，比较后丢弃一半 → O(log n)。
// 本文件给出基本版，以及两个极常用的变体：下界(lower_bound)与上界(upper_bound)。
#pragma once

#include <vector>

namespace alg {

// 找到任一等于 target 的下标；不存在返回 -1
int binarySearch(const std::vector<int>& a, int target) {
    int lo = 0, hi = (int)a.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;           // 防溢出：不写 (lo+hi)/2
        if (a[mid] == target) return mid;
        if (a[mid] < target) lo = mid + 1; else hi = mid - 1;
    }
    return -1;
}

// lower_bound：第一个 >= target 的下标；没有则 a.size()（可插入位置）
int lowerBound(const std::vector<int>& a, int target) {
    int lo = 0, hi = (int)a.size();
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] < target) lo = mid + 1; else hi = mid;
    }
    return lo;
}

// upper_bound：第一个 > target 的下标
int upperBound(const std::vector<int>& a, int target) {
    int lo = 0, hi = (int)a.size();
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] <= target) lo = mid + 1; else hi = mid;
    }
    return lo;
}

} // namespace alg