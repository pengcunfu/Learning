// sorting.hpp —— 常用排序算法（对应 doc：04_算法设计/02_排序算法 各文件）
//
// 收录：冒泡、选择、插入、希尔、归并、快排、堆排序（比较型）+ 计数、基数（非比较型）。
// 复杂度速记：
//   简单型(冒泡/选择/插入) O(n^2)；希尔 ~O(n^1.3)；
//   高效型(归并/快排/堆) O(n log n)；计数/基数 O(n+k)。
#pragma once

#include <algorithm>
#include <vector>

namespace alg {

// 冒泡：相邻比较交换，最大的像气泡浮到末尾
void bubbleSort(std::vector<int>& a) {
    int n = a.size();
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j)
            if (a[j] > a[j + 1]) { std::swap(a[j], a[j + 1]); swapped = true; }
        if (!swapped) break;      // 已有序则提前结束
    }
}

// 选择：每轮从无序区挑最小放到前面
void selectionSort(std::vector<int>& a) {
    int n = a.size();
    for (int i = 0; i < n - 1; ++i) {
        int mn = i;
        for (int j = i + 1; j < n; ++j) if (a[j] < a[mn]) mn = j;
        std::swap(a[i], a[mn]);
    }
}

// 插入：像扑克牌，把当前元素插入前面已排序区
void insertionSort(std::vector<int>& a) {
    int n = a.size();
    for (int i = 1; i < n; ++i) {
        int key = a[i], j = i - 1;
        while (j >= 0 && a[j] > key) { a[j + 1] = a[j]; --j; }
        a[j + 1] = key;
    }
}

// 希尔：插入排序的改进，按"间隔"分组预排，间隔逐步缩小到1
void shellSort(std::vector<int>& a) {
    int n = a.size();
    for (int gap = n / 2; gap > 0; gap /= 2)
        for (int i = gap; i < n; ++i) {
            int key = a[i], j = i;
            while (j >= gap && a[j - gap] > key) { a[j] = a[j - gap]; j -= gap; }
            a[j] = key;
        }
}

// 归并（分治）：把两段有序序列合并；先分到底再回程合并
void mergeSort(std::vector<int>& a) {
    std::vector<int> tmp(a.size());
    struct Merge {
        std::vector<int>& a; std::vector<int>& tmp;
        void run(int lo, int hi) {        // [lo,hi]
            if (lo >= hi) return;
            int mid = (lo + hi) / 2;
            run(lo, mid); run(mid + 1, hi);
            int i = lo, j = mid + 1, k = lo;
            while (i <= mid && j <= hi) tmp[k++] = a[i] <= a[j] ? a[i++] : a[j++];
            while (i <= mid) tmp[k++] = a[i++];
            while (j <= hi) tmp[k++] = a[j++];
            for (i = lo; i <= hi; ++i) a[i] = tmp[i];
        }
    } m{a, tmp};
    m.run(0, (int)a.size() - 1);
}

// 快排（分治）：选基准，小的去左、大的去右，再递归两边
void quickSort(std::vector<int>& a) {
    struct Q {
        std::vector<int>& a;
        void run(int lo, int hi) {
            if (lo >= hi) return;
            int pivot = a[lo + (hi - lo) / 2];   // 三数取左中右，或任取
            int l = lo, r = hi;
            while (l <= r) {
                while (a[l] < pivot) ++l;
                while (a[r] > pivot) --r;
                if (l <= r) { std::swap(a[l], a[r]); ++l; --r; }
            }
            run(lo, r); run(l, hi);
        }
    } q{a};
    q.run(0, (int)a.size() - 1);
}

// 堆排序：先建最大堆，反复把堆顶(最大)放到末尾
void heapSort(std::vector<int>& a) {
    // 见 ds/binary_heap：用其静态 siftDown 建堆+调整
    auto siftDown = [&a](size_t n, size_t i) {
        while (true) {
            size_t lg = i, l = 2 * i + 1, r = 2 * i + 2;
            if (l < n && a[l] > a[lg]) lg = l;
            if (r < n && a[r] > a[lg]) lg = r;
            if (lg == i) break;
            std::swap(a[i], a[lg]); i = lg;
        }
    };
    for (size_t i = a.size() / 2; i-- > 0;) siftDown(a.size(), i);
    for (size_t n = a.size(); n > 1; --n) {
        std::swap(a[0], a[n - 1]);
        siftDown(n - 1, 0);
    }
}

// 计数排序（非比较）：适合值域 k 不大的非负整数
std::vector<int> countingSort(const std::vector<int>& a, int k) {
    std::vector<int> cnt(k + 1, 0), out(a.size());
    for (int v : a) ++cnt[v];
    for (int i = 1; i <= k; ++i) cnt[i] += cnt[i - 1];    // 前缀和 → 稳定
    for (auto it = a.rbegin(); it != a.rend(); ++it) out[--cnt[*it]] = *it;
    return out;
}

// 基数排序（非比较）：按个位、十位…逐位做计数排序
void radixSort(std::vector<int>& a) {
    if (a.empty()) return;
    int mx = *std::max_element(a.begin(), a.end());
    std::vector<int> out(a.size());
    for (int exp = 1; mx / exp > 0; exp *= 10) {
        int cnt[10] = {0};
        for (int v : a) ++cnt[(v / exp) % 10];
        for (int i = 1; i < 10; ++i) cnt[i] += cnt[i - 1];
        for (auto it = a.rbegin(); it != a.rend(); ++it) out[--cnt[(*it / exp) % 10]] = *it;
        a = out;
    }
}

} // namespace alg