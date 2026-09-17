

# 弗洛伊德算法

**克鲁斯卡尔算法**（Kruskal's Algorithm）是另一种解决 **最小生成树**（MST）问题的贪心算法。与 **普里姆算法** 不同，克鲁斯卡尔算法的核心思想是通过边的排序来构建最小生成树。它通过将图中的边按权值排序，然后逐步选择边加入生成树，直到构建完成。

### 算法原理

1. 将图中的所有边按照权值升序排序。
2. 从最小的边开始，逐一检查每条边，选择不形成环的边，加入生成树。
3. 重复此过程，直到生成树中包含 V−1V-1 条边（VV 是图中的顶点数）。

### 关键点

- **并查集（Union-Find）**：用来判断加入的边是否会形成环。并查集的数据结构有两个操作：**查找**（Find）和**合并**（Union）。通过查找操作可以检测两个顶点是否属于同一集合，合并操作则将两个集合合并成一个。

### 算法步骤

1. 对所有边按权值进行排序。
2. 初始化一个并查集，每个顶点为一个独立的集合。
3. 依次处理所有边，若当前边的两个端点不在同一集合中，则将该边加入生成树，并合并这两个端点所在的集合。
4. 重复步骤 3，直到生成树中包含 V−1V-1 条边。

### 算法的实现

我们可以使用 **邻接边列表** 来表示图，利用 **并查集** 来检查是否形成环。

#### 基于并查集的克鲁斯卡尔算法实现

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Graph {
public:
    // 存储图的边
    struct Edge {
        int u, v, weight;
        bool operator<(const Edge& other) const {
            return weight < other.weight;  // 按边的权重排序
        }
    };

    int V;  // 顶点数
    vector<Edge> edges;  // 图的所有边

    // 构造函数，初始化顶点数
    Graph(int V) : V(V) {}

    // 添加边
    void addEdge(int u, int v, int weight) {
        edges.push_back({u, v, weight});
    }

    // 并查集查找操作
    int find(int parent[], int i) {
        if (parent[i] == -1) {
            return i;  // 如果当前节点是根节点，返回自己
        }
        return find(parent, parent[i]);  // 否则递归查找
    }

    // 并查集合并操作
    void Union(int parent[], int x, int y) {
        int xroot = find(parent, x);
        int yroot = find(parent, y);
        if (xroot != yroot) {
            parent[xroot] = yroot;  // 将一个集合的根节点指向另一个根节点
        }
    }

    // 克鲁斯卡尔算法求最小生成树
    void kruskal() {
        vector<Edge> mst;  // 最小生成树的边集合

        // 将所有边按权重升序排序
        sort(edges.begin(), edges.end());

        // 初始化并查集
        int parent[V];
        fill(parent, parent + V, -1);

        // 处理所有边
        for (auto& edge : edges) {
            int u = edge.u;
            int v = edge.v;

            // 检查是否会形成环
            int set_u = find(parent, u);
            int set_v = find(parent, v);

            // 如果u和v不在同一个集合中，说明不会形成环，可以加入最小生成树
            if (set_u != set_v) {
                mst.push_back(edge);
                Union(parent, set_u, set_v);
            }

            // 如果最小生成树已经包含V-1条边，算法结束
            if (mst.size() == V - 1) break;
        }

        // 输出最小生成树的边
        cout << "最小生成树的边：\n";
        int totalWeight = 0;
        for (auto& edge : mst) {
            cout << edge.u << " - " << edge.v << " 权值: " << edge.weight << endl;
            totalWeight += edge.weight;
        }
        cout << "最小生成树的总权值: " << totalWeight << endl;
    }
};

int main() {
    Graph g(5);

    // 添加边，表示一个带权无向图
    g.addEdge(0, 1, 2);
    g.addEdge(0, 3, 6);
    g.addEdge(1, 2, 3);
    g.addEdge(1, 3, 8);
    g.addEdge(1, 4, 5);
    g.addEdge(2, 4, 7);
    g.addEdge(3, 4, 9);

    // 执行克鲁斯卡尔算法，输出最小生成树
    g.kruskal();

    return 0;
}
```

### 代码解释

1. **图的表示**：
   - 使用一个结构体 `Edge` 来表示图的每一条边。`Edge` 包含三个字段：`u` 和 `v` 为边的两个端点，`weight` 为边的权值。
   - `edges` 是一个存储所有边的向量。
2. **并查集操作**：
   - `find` 函数实现并查集的查找操作，采用递归方式查找某个顶点的根节点。
   - `Union` 函数实现并查集的合并操作，将两个集合合并。
3. **克鲁斯卡尔算法**：
   - 对所有边按权值进行排序。
   - 逐一处理边，如果边的两个端点不在同一个集合中，就将该边加入生成树，并合并这两个端点所在的集合。
   - 重复此过程直到生成树包含 V−1V-1 条边。
4. **输出**：
   - 最后输出最小生成树的所有边及其权值，并计算总权值。

### 输出示例

```
最小生成树的边：
0 - 1 权值: 2
1 - 2 权值: 3
0 - 3 权值: 6
1 - 4 权值: 5
最小生成树的总权值: 16
```

### 时间复杂度

- **排序边**：排序边的时间复杂度是 O(Elog⁡E)O(E \log E)，其中 EE 是图中的边数。
- **并查集操作**：每次查找和合并的时间复杂度是 O(α(V))O(\alpha(V))，其中 VV 是顶点数，α\alpha 是阿克曼函数的反函数，增长速度非常慢，接近常数时间。

因此，总的时间复杂度为 O(Elog⁡E+Eα(V))O(E \log E + E \alpha(V))，其中 Elog⁡EE \log E 是主导项，通常可以简化为 O(Elog⁡E)O(E \log E)。

### 空间复杂度

- 空间复杂度为 O(V+E)O(V + E)，需要存储图的所有边和并查集的父节点数组。

### 总结

克鲁斯卡尔算法是一种基于边的排序来构建最小生成树的算法，适用于稀疏图的最小生成树求解。它的核心优势在于每次都选择最小的边并检查是否形成环，确保最终得到的生成树是最优的。