// dijkstra.hpp —— 单源最短路径（对应 doc：03_图论/03_最短路径-Dijkstra算法）
//
// 适用于：非负权图，求一个源点到所有点的最短距离。
// 思想（贪心 + 松弛）：每次从未确定点里挑"当前距离最小"的（贪心），拿它去松弛邻居。
// 因为权非负，被挑出的点的距离已是最终最短。用优先队列（最小堆）优化"挑最小"。
#pragma once

#include <queue>
#include <utility>
#include <vector>

#include "../ds/priority_queue.hpp"
#include "graph.hpp"

namespace alg {

constexpr int INF_DIST = 1e9;

struct DijkstraResult {
    std::vector<int> dist;  // dist[i] = 源点到 i 的最短距离；不可达为 INF_DIST
    std::vector<int> prev;  // 前驱（还原路径用）；起点/无前驱为 -1
    bool reachable(int i) const { return dist[i] < INF_DIST; }
};

// 返回源点 s 到所有点的最短距离与前驱
DijkstraResult dijkstra(const WeightedGraph& g, int s) {
    int n = g.size();
    DijkstraResult r{std::vector<int>(n, INF_DIST), std::vector<int>(n, -1)};
    r.dist[s] = 0;

    // 小顶堆（first 是距离）：每次弹出全局最小，即贪心选择的那个点
    ds::PriorityQueue<std::pair<int, int>, std::greater<std::pair<int, int>>> pq;
    pq.push({0, s});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > r.dist[u]) continue;   // 惰性删除：这条是旧记录，作废
        for (const WEdge& e : g.neighbours(u)) {
            int nd = d + e.w;          // 松弛：若经 u 到 e.to 更短则更新
            if (nd < r.dist[e.to]) {
                r.dist[e.to] = nd;
                r.prev[e.to] = u;
                pq.push({nd, e.to});
            }
        }
    }
    return r;
}

} // namespace alg