// graph.hpp —— 图结构与遍历（对应 doc：03_图论/01_图的基础理论）
//
// 存储方式两种：
//   - 邻接表：每个顶点一个 vector，只存邻居 —— 省空间、适合稀疏图
//   - 邻接矩阵：VxV，op[i][j]=1/权 —— 查边 O(1)、适合稠密图
// 本文件实现邻接表 + BFS / DFS。BFS 用队列保证按层推进，DFS（对无权图）能先
// 得到距起点步数；DFS 用递归或显式栈。
#pragma once

#include <queue>
#include <stack>
#include <utility>
#include <vector>

namespace alg {

// 无权图（顶点 0..n-1），邻接表存储
class Graph {
public:
    explicit Graph(int n) : adj_(n) {}

    void addEdge(int u, int v) { adj_[u].push_back(v); }

    // BFS：从 s 出发按层扩展（队列）。返回访问顺序，并自动记录各点距 s 的步数。
    std::vector<int> bfs(int s) {
        std::vector<int> order;
        lastDist_.assign(size(), -1);
        std::queue<int> q;
        lastDist_[s] = 0; q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            order.push_back(u);
            for (int v : adj_[u])
                if (lastDist_[v] == -1) { lastDist_[v] = lastDist_[u] + 1; q.push(v); }
        }
        return order;
    }
    // BFS 之后取各点到 s 的最短步数（无权图 BFS 天然求最短路）
    std::vector<int> distances() const { return lastDist_; }

    // DFS 递归版
    std::vector<int> dfs(int s) {
        std::vector<int> order;
        std::vector<bool> vis(size(), false);
        dfs_(s, vis, order);
        return order;
    }
    // DFS 迭代版（显式栈代替递归）
    std::vector<int> dfsIterative(int s) {
        std::vector<int> order;
        std::vector<bool> vis(size(), false);
        std::stack<int> st; st.push(s);
        while (!st.empty()) {
            int u = st.top(); st.pop();
            if (vis[u]) continue;
            vis[u] = true; order.push_back(u);
            for (auto it = adj_[u].rbegin(); it != adj_[u].rend(); ++it)
                if (!vis[*it]) st.push(*it);
        }
        return order;
    }

    int size() const { return static_cast<int>(adj_.size()); }
    const std::vector<int>& neighbours(int u) const { return adj_[u]; }

private:
    std::vector<std::vector<int>> adj_;
    std::vector<int> lastDist_;

    void dfs_(int u, std::vector<bool>& vis, std::vector<int>& order) {
        vis[u] = true; order.push_back(u);
        for (int v : adj_[u]) if (!vis[v]) dfs_(v, vis, order);
    }
};

// 带权边
struct WEdge { int to; int w; };

// 带权图（邻接表：{(邻居, 权值)}），供 Dijkstra / Prim / Kruskal / 拓扑 使用
class WeightedGraph {
public:
    explicit WeightedGraph(int n) : adj_(n) {}
    void addDirected(int u, int v, int w) { adj_[u].push_back({v, w}); }
    void addUndirected(int u, int v, int w) { addDirected(u, v, w); addDirected(v, u, w); }
    int size() const { return static_cast<int>(adj_.size()); }
    const std::vector<WEdge>& neighbours(int u) const { return adj_[u]; }
private:
    std::vector<std::vector<WEdge>> adj_;
};

} // namespace alg