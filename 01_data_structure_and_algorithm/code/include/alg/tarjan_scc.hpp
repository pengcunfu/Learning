// tarjan_scc.hpp —— 强连通分量（对应 doc：03_图论/06_连通性-强连通分量）
//
// 概念：有向图中，两两互相可达的顶点构成一个强连通分量(SCC)。
// Tarjan 用一次 DFS 求出所有 SCC：维护每个点的 dfn(访问序) 与 low(能回到的最小 dfn)。
// 当 u 出栈时 low[u]==dfn[u]，则栈顶到 u 之间正是当前栈上属于一个 SCC 的顶点。
// 应用：缩点成 DAG、判断2-SAT可满足、环检测。
#pragma once

#include <algorithm>
#include <vector>

namespace alg {

struct SCCResult {
    int count = 0;                     // 强连通分量个数
    std::vector<int> comp;             // 每个顶点属于第几个分量
};

// 返回强连通分量个数与每个点所属分量编号
SCCResult tarjan(const std::vector<std::vector<int>>& adj) {
    int n = adj.size(), t = 0;
    SCCResult r;
    r.comp.assign(n, -1);
    std::vector<int> dfn(n, -1), low(n, 0), stk;
    std::vector<bool> inStack(n, false);

    struct Dfs {
        int& t; std::vector<int>& dfn; std::vector<int>& low;
        std::vector<int>& stk; std::vector<bool>& inStack;
        const std::vector<std::vector<int>>& adj;
        SCCResult& r;
        void run(int u) {
            dfn[u] = low[u] = t++;
            stk.push_back(u); inStack[u] = true;
            for (int v : adj[u]) {
                if (dfn[v] == -1) { run(v); low[u] = std::min(low[u], low[v]); }
                else if (inStack[v]) low[u] = std::min(low[u], dfn[v]); // 回边/横叉到栈上
            }
            if (low[u] == dfn[u]) {     // 找到一个新的 SCC 根
                int v;
                do {
                    v = stk.back(); stk.pop_back(); inStack[v] = false;
                    r.comp[v] = r.count;
                } while (v != u);
                ++r.count;
            }
        }
    };
    Dfs d{t, dfn, low, stk, inStack, adj, r};
    for (int i = 0; i < n; ++i) if (dfn[i] == -1) d.run(i);
    return r;
}

} // namespace alg