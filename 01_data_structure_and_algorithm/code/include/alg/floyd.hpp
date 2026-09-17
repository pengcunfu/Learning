// floyd.hpp —— 多源最短路径（对应 doc：03_图论/04_多源最短路径-Floyd算法）
//
// 思想（动态规划）：用"中间点"递推。dp[k][i][j] = 只允许途经 0..k 这些点的
// i→j 最短距离。状态转移：要么不经 k（dp[k-1][i][j]），要么经 k（dp[k-1][i][k]+dp[k-1][k][j]）。
// 滚动优化后就地更新矩阵，O(V^3)，适合小规模、负权（无负环）图的最短路。
#pragma once

#include <algorithm>
#include <vector>

namespace alg {

constexpr int INF_FL = 1e9;

// 返回每对点的最短距离矩阵 dist（dist[i][i]=0，不可达为 INF_FL）
// 入参矩阵 g：g[i][j] = 边权，无边为 INF_FL，对角线为 0。
std::vector<std::vector<int>> floyd(const std::vector<std::vector<int>>& g) {
    int n = g.size();
    std::vector<std::vector<int>> dist = g;
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (dist[i][k] < INF_FL && dist[k][j] < INF_FL)
                    dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
    return dist;
}

} // namespace alg