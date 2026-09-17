// backtracking.hpp —— 回溯算法（对应 doc：04_算法设计/01_算法思想/回溯算法 及题解）
//
// 回溯 = DFS + 撤销现场：先临时放入一个选择 → 递归 → 再撤销，穷举所有可能，
// 并在搜索中剪枝以提前放弃不可能的分支。典型：N皇后、全排列、组合求和。
#pragma once

#include <string>
#include <vector>

namespace alg {

// N 皇后：返回所有可行摆法（每行一个皇后，下标=列）
std::vector<std::vector<int>> nQueens(int n) {
    std::vector<std::vector<int>> res;
    std::vector<int> col(n);
    std::vector<bool> used(n, false), diag1(2 * n, false), diag2(2 * n, false);

    auto go = [&](auto&& self, int row) -> void {
        if (row == n) { res.push_back(col); return; }
        for (int c = 0; c < n; ++c) {
            int d1 = row - c + n, d2 = row + c;      // 两条对角线编号
            if (used[c] || diag1[d1] || diag2[d2]) continue; // 剪枝
            col[row] = c; used[c] = diag1[d1] = diag2[d2] = true;
            self(self, row + 1);                      // 递归下一行
            used[c] = diag1[d1] = diag2[d2] = false;  // 撤销现场
        }
    };
    go(go, 0);
    return res;
}

// 全排列：把 nums 的所有排列返回
std::vector<std::vector<int>> permutations(std::vector<int> nums) {
    std::vector<std::vector<int>> res;
    auto go = [&](auto&& self, int start) -> void {
        if (start == (int)nums.size()) { res.push_back(nums); return; }
        for (int i = start; i < (int)nums.size(); ++i) {
            std::swap(nums[start], nums[i]);
            self(self, start + 1);
            std::swap(nums[start], nums[i]);   // 撤销交换
        }
    };
    go(go, 0);
    return res;
}

// 组合求和(可重复选)：candidates 里选若干个数(可重复)恰好等于 target，返回所有组合（去重）
std::vector<std::vector<int>> combinationSum(std::vector<int> candidates, int target) {
    std::vector<std::vector<int>> res;
    std::vector<int> cur;
    std::sort(candidates.begin(), candidates.end());
    auto go = [&](auto&& self, int start, int remain) -> void {
        if (remain < 0) return;
        if (remain == 0) { res.push_back(cur); return; }
        for (int i = start; i < (int)candidates.size(); ++i) {
            if (i > start && candidates[i] == candidates[i - 1]) continue; // 去重
            cur.push_back(candidates[i]);
            self(self, i, remain - candidates[i]);  // 可重复，继续从 i 开始
            cur.pop_back();                          // 撤销
        }
    };
    go(go, 0, target);
    return res;
}

} // namespace alg