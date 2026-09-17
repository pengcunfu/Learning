



# 普里姆算法

**普里姆算法**（Prim's Algorithm）是解决**最小生成树**问题的一种贪心算法。给定一个带权无向图，最小生成树（MST）是图中一棵包含所有节点的树，且树中边的权重之和最小。

### 算法原理

普里姆算法的基本思想是从一个顶点开始，逐步扩展到其他顶点，每次选择权值最小的边连接一个新的顶点，直到所有的顶点都被包含在生成树中。

具体步骤如下：

1. 从任意一个顶点开始，将其标记为已经加入生成树。
2. 找到一个与生成树中的顶点相连、且权值最小的边，将该边及其对应的顶点加入生成树。
3. 重复步骤 2，直到所有的顶点都被加入生成树。

### 算法步骤

1. 初始化一个集合，包含图中的一个顶点，并标记该顶点为已加入生成树。
2. 每次从已加入生成树的顶点集合中，选取一条权重最小的边，且该边的另一个端点未在生成树中。
3. 将边的另一个端点加入生成树，并重复第 2 步，直到所有顶点都被加入生成树。

### 算法的实现

我们使用 **邻接矩阵** 来表示图，使用 **优先队列** 来选择权值最小的边。

#### 基于邻接矩阵的普里姆算法实现

```cpp
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

#define INF INT_MAX  // 定义无穷大

// 使用邻接矩阵表示图
class Graph {
private:
    int V;  // 顶点数
    vector<vector<int>> adjMatrix;  // 邻接矩阵

public:
    // 构造函数，初始化图的顶点数和邻接矩阵
    Graph(int V) : V(V) {
        adjMatrix.resize(V, vector<int>(V, INF));  // 初始化矩阵为INF
    }

    // 添加边
    void addEdge(int u, int v, int weight) {
        adjMatrix[u][v] = weight;
        adjMatrix[v][u] = weight;
    }

    // 普里姆算法求最小生成树
    void prim() {
        vector<int> key(V, INF);  // key[i]存储从生成树到i的最小边权
        vector<bool> inMST(V, false);  // inMST[i]表示顶点i是否已经在最小生成树中
        vector<int> parent(V, -1);  // parent[i]记录顶点i的父节点

        key[0] = 0;  // 从第一个顶点开始
        parent[0] = -1;  // 第一个顶点没有父节点

        for (int count = 0; count < V - 1; count++) {
            // 选择一个未在生成树中的顶点，且key值最小的顶点
            int u = -1;
            int minKey = INF;
            for (int v = 0; v < V; v++) {
                if (!inMST[v] && key[v] < minKey) {
                    minKey = key[v];
                    u = v;
                }
            }

            inMST[u] = true;  // 将u加入生成树

            // 更新u的邻接点的key值
            for (int v = 0; v < V; v++) {
                if (adjMatrix[u][v] != INF && !inMST[v] && adjMatrix[u][v] < key[v]) {
                    key[v] = adjMatrix[u][v];
                    parent[v] = u;
                }
            }
        }

        // 输出最小生成树
        cout << "最小生成树的边：\n";
        for (int i = 1; i < V; i++) {
            cout << parent[i] << " - " << i << " 权值: " << adjMatrix[i][parent[i]] << endl;
        }
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

    // 执行普里姆算法，输出最小生成树
    g.prim();

    return 0;
}
```

### 代码解释

1. **图的表示**：
   - 使用邻接矩阵来表示图，`adjMatrix[u][v]` 存储顶点 `u` 和顶点 `v` 之间的边的权值。如果没有边，则存储为 `INF`。
2. **普里姆算法实现**：
   - `key[]` 数组存储从生成树到其他顶点的最小边的权值。
   - `inMST[]` 数组表示顶点是否已经在生成树中。
   - `parent[]` 数组记录每个顶点的父节点，用于输出生成树的边。
3. **主要操作**：
   - 在每一步，选择一个未在生成树中的顶点，找到其与生成树相连的权值最小的边。
   - 然后将该顶点添加到生成树，并更新与其相连的顶点的 `key` 值。
4. **输出**：
   - 最后输出生成树的所有边及其权值。

### 输出示例

```
最小生成树的边：
0 - 1 权值: 2
1 - 2 权值: 3
0 - 3 权值: 6
1 - 4 权值: 5
```

### 时间复杂度

- **时间复杂度**：每次都要遍历所有的顶点和边，使用邻接矩阵时，时间复杂度为 O(V2)O(V^2)，其中 VV 是顶点的数量。如果使用堆优化，可以将时间复杂度降到 O(Elog?V)O(E \log V)，其中 EE 是边的数量。
- **空间复杂度**：需要存储图的邻接矩阵，所以空间复杂度为 O(V2)O(V^2)。

### 总结

普里姆算法是一种贪心算法，通过不断选择最小的边来扩展生成树，适用于稠密图的最小生成树求解。

最短路径