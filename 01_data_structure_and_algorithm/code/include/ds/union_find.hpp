// union_find.hpp —— 并查集 / 不相交集合（对应 doc：04_算法设计/03_查找算法/并查集Union-Find）
//
// 概念：维护若干不相交集合，支持"查询元素所在集合"与"合并两个集合"。
// 两个优化让它近似 O(1)：
//   - 路径压缩：find 时把沿途节点直接挂到根上。
//   - 按秩合并：总是把矮树挂到高树下面。
// 典型应用：Kruskal 最小生成树、连通分量判断、查血缘关系。
#pragma once

#include <algorithm>
#include <vector>

namespace ds {

class UnionFind {
public:
    explicit UnionFind(int n) : parent_(n), rank_(n, 0), sets_(n) {
        for (int i = 0; i < n; ++i) parent_[i] = i;
    }

    // 找 x 的根，并做路径压缩
    int find(int x) {
        if (parent_[x] != x) parent_[x] = find(parent_[x]);
        return parent_[x];
    }

    bool connected(int a, int b) { return find(a) == find(b); }

    // 合并 a、b 所在集合，返回是否发生了合并（原本不在同一集合）
    bool unify(int a, int b) {
        int ra = find(a), rb = find(b);
        if (ra == rb) return false;
        if (rank_[ra] < rank_[rb]) std::swap(ra, rb);
        parent_[rb] = ra;
        if (rank_[ra] == rank_[rb]) ++rank_[ra];
        --sets_;
        return true;
    }

    int count() const { return sets_; }
    int size() const { return static_cast<int>(parent_.size()); }

private:
    std::vector<int> parent_;
    std::vector<int> rank_;
    int sets_ = 0;
};

} // namespace ds