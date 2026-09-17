// kruskal.hpp —— 最小生成树：Kruskal 算法（对应 doc：03_图论/09_最小生成树-Kruskal算法）
//
// 思想（贪心 + 加边 + 判环）：把所有边按权从小到大排序，逐条尝试，
// 只要这条边两端不在同一集合（用并查集判断，不成环）就加入生成树。
// 区别：Prim 面向顶点加点，Kruskal 面向边加边（适合稀疏图）。
#pragma once

#include <algorithm>
#include <vector>

#include "../ds/union_find.hpp"
#include "graph.hpp"

namespace alg {

struct Edge { int u, v, w; };

// 返回最小生成树总权值；不连通返回 -1
int kruskal(int n, std::vector<Edge>& edges) {
    std::sort(edges.begin(), edges.end(),
              [](const Edge& a, const Edge& b) { return a.w < b.w; });
    ds::UnionFind uf(n);
    int total = 0, added = 0;
    for (const Edge& e : edges) {
        if (uf.unify(e.u, e.v)) {   // 不成环才合并（集合不同才会合并）
            total += e.w;
            if (++added == n - 1) break;   // n-1 条边即完成
        }
    }
    return added == n - 1 ? total : -1;
}

} // namespace alg