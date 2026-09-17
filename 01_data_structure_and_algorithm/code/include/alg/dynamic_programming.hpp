// dynamic_programming.hpp —— 动态规划（对应 doc：04_算法设计/01_算法思想/动态规划 及题解）
//
// DP 三板斧：定义状态 → 写转移方程 → 定边界。
// 本文件给出 4 个最经典的入门例子，体会"用空间换时间 / 自底向上填表"。
#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace alg {

// 斐波那契：dp[i]=dp[i-1]+dp[i-2]，去掉重复递归
long long fibDp(int n) {
    if (n <= 1) return n;
    std::vector<long long> dp(n + 1);
    dp[0] = 0; dp[1] = 1;
    for (int i = 2; i <= n; ++i) dp[i] = dp[i - 1] + dp[i - 2];
    return dp[n];
}

// 最长递增子序列 LIS：dp[i] = 以 a[i] 结尾的最长递增子序列长度
int lis(const std::vector<int>& a) {
    int n = a.size();
    std::vector<int> dp(n, 1);
    int best = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < i; ++j)
            if (a[j] < a[i]) dp[i] = std::max(dp[i], dp[j] + 1);
        best = std::max(best, dp[i]);
    }
    return best;
}

// 0-1 背包：有 n 件物品(weight[i],value[i])，容量 cap，求最大价值（每件最多选1次）
// dp[j] = 容量 j 能装下的最大价值；dp[j]=max(dp[j], dp[j-w]+v)（倒序防重复取）
int knapsack01(const std::vector<int>& w, const std::vector<int>& v, int cap) {
    std::vector<int> dp(cap + 1, 0);
    for (int i = 0; i < (int)w.size(); ++i)
        for (int j = cap; j >= w[i]; --j)          // 倒序是 0-1 背包的关键
            dp[j] = std::max(dp[j], dp[j - w[i]] + v[i]);
    return dp[cap];
}

// 最长公共子序列 LCS：dp[i][j] = a[0..i) 与 b[0..j) 的 LCS 长度
int lcs(const std::string& a, const std::string& b) {
    int n = a.size(), m = b.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j) {
            if (a[i - 1] == b[j - 1]) dp[i][j] = dp[i - 1][j - 1] + 1;
            else dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
        }
    return dp[n][m];
}

} // namespace alg