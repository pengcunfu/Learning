// topological_sort.hpp —— 拓扑排序（对应 doc：03_图论/05_排序算法-拓扑排序 与 04_算法设计/排序）
//
// 适用：有向无环图（DAG），把顶点排成一个线性序列，使得每条边 u→v 中 u 都在 v 前。
// 典型：课程先修顺序、构建/依赖排序、编译依赖。
// 两种方法：
//   - Kahn（BFS 思路）：反复取出"入度为 0"的顶点，删除其出边。有环则排不满 → 判环。
//   - DFS：后序的逆序即拓扑序；递归栈中再遇未完成的点说明有环。
#pragma once

#include <algorithm>
#include <queue>
#include <vector>

namespace alg {

// Kahn 算法：返回一个合法拓扑序；若存在环返回 empty。
std::vector<int> topologicalSortKahn(const std::vector<std::vector<int>>& adj) {
    int n = adj.size();
    std::vector<int> indeg(n, 0);
    for (int u = 0; u < n; ++u)
        for (int v : adj[u]) ++indeg[v];
    std::queue<int> q;
    for (int i = 0; i < n; ++i) if (indeg[i] == 0) q.push(i);
    std::vector<int> order;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : adj[u]) if (--indeg[v] == 0) q.push(v);
    }
    if ((int)order.size() != n) return {};   // 有环：排不满
    return order;
}

// DFS 法：后序逆序。返回 true 表示无环。
bool topologicalSortDFS(const std::vector<std::vector<int>>& adj, std::vector<int>& order) {
    int n = adj.size();
    std::vector<int> state(n, 0);      // 0 未访问 1 访问中(栈上) 2 完成
    order.clear();
    bool acyclic = true;
    struct Dfs {
        const std::vector<std::vector<int>>& adj;
        std::vector<int>& state;
        std::vector<int>& order;
        bool& ok;
        void run(int u) {
            state[u] = 1;
            for (int v : adj[u]) {
                if (state[v] == 1) { ok = false; return; }   // 回边 → 有环
                if (state[v] == 0) run(v);
            }
            state[u] = 2;
            order.push_back(u);
        }
    };
    Dfs d{adj, state, order, acyclic};
    for (int i = 0; i < n && acyclic; ++i) if (state[i] == 0) d.run(i);
    if (!acyclic) return false;
    std::reverse(order.begin(), order.end());   // 后序逆序
    return true;
}

} // namespace alg