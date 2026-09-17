// prim.hpp —— 最小生成树：Prim 算法（对应 doc：03_图论/02_最小生成树-Prim算法）
//
// 目标：无向连通带权图，挑出 n-1 条边连接所有顶点且总权最小。
// Prim 思想（贪心 + 加点）：从任一顶点开始，不断选取"已选集合 到 未选集合 的最短边"，
// 把新顶点并入集合。用最小堆加速选"最短边"。返回最小生成树总权值。
#pragma once

#include <numeric>
#include <utility>
#include <vector>

#include "../ds/priority_queue.hpp"
#include "graph.hpp"

namespace alg {

constexpr int INF_PRIM = 1e9;

// 返回最小生成树总权值；若图不连通返回 -1
int prim(const WeightedGraph& g) {
    int n = g.size();
    std::vector<bool> inTree(n, false);
    std::vector<int> key(n, INF_PRIM);   // 到已选集合的最短边权
    key[0] = 0;
    ds::PriorityQueue<std::pair<int, int>, std::greater<std::pair<int, int>>> pq;
    pq.push({0, 0});
    int total = 0, picked = 0;
    while (!pq.empty()) {
        auto [w, u] = pq.top(); pq.pop();
        if (inTree[u]) continue;
        if (w != key[u]) continue;       // 惰性删除
        inTree[u] = true; total += w; ++picked;
        for (const WEdge& e : g.neighbours(u))
            if (!inTree[e.to] && e.w < key[e.to]) {
                key[e.to] = e.w;
                pq.push({e.w, e.to});
            }
    }
    return picked == n ? total : -1;     // 是不是连了所有点
}

} // namespace alg