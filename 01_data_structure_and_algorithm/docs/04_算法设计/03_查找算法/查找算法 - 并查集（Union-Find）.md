并查集（Union-Find）是一种用于处理元素分组问题的数据结构，广泛应用于解决连通性问题，尤其是在图算法中，如求解最小生成树、网络连接等问题。

并查集主要支持两种操作：

1. **查找（Find）**：找到元素所在的集合。
2. **合并（Union）**：将两个元素所在的集合合并成一个集合。

并查集通常通过两种技术优化：

- **路径压缩（Path Compression）**：在进行查找操作时，直接将查找路径上的所有节点指向根节点，以加速后续的查找操作。
- **按秩合并（Union by Rank/Size）**：将小的树挂到大的树下面，以减少树的高度，从而提高查找效率。

### C++ 实现并查集

#### 1. 并查集的基本实现

```cpp
#include <iostream>
#include <vector>

using namespace std;

class UnionFind {
private:
    vector<int> parent;  // 存储每个元素的父节点
    vector<int> rank;    // 存储每个元素的秩（树的深度）

public:
    // 构造函数：初始化并查集
    UnionFind(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        // 初始化每个元素的父节点为其本身
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    // 查找操作：返回x所在集合的根节点
    int find(int x) {
        if (parent[x] != x) {
            // 路径压缩：递归查找并将路径上的所有节点指向根节点
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    // 合并操作：将x和y所在的集合合并
    void unionSets(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);

        if (rootX != rootY) {
            // 按秩合并：将秩较小的树合并到秩较大的树下面
            if (rank[rootX] > rank[rootY]) {
                parent[rootY] = rootX;
            } else if (rank[rootX] < rank[rootY]) {
                parent[rootX] = rootY;
            } else {
                parent[rootY] = rootX;
                rank[rootX]++;  // 如果秩相同，合并后增加根节点的秩
            }
        }
    }

    // 打印并查集的父节点和秩（用于调试）
    void print() {
        for (int i = 0; i < parent.size(); i++) {
            cout << "Element " << i << ": Parent = " << parent[i] << ", Rank = " << rank[i] << endl;
        }
    }
};

int main() {
    // 创建一个包含10个元素的并查集
    UnionFind uf(10);

    // 合并元素
    uf.unionSets(1, 2);
    uf.unionSets(2, 3);
    uf.unionSets(4, 5);
    uf.unionSets(6, 7);
    uf.unionSets(8, 9);

    // 打印并查集的状态
    uf.print();

    // 查找操作
    cout << "Find(1): " << uf.find(1) << endl;  // 输出1的根节点
    cout << "Find(5): " << uf.find(5) << endl;  // 输出5的根节点

    // 合并操作
    uf.unionSets(3, 4);

    // 再次打印并查集的状态
    uf.print();

    return 0;
}
```

### 2. 解释

#### 2.1. **构造函数**

`UnionFind(int n)` 构造函数接受一个整数 `n`，表示并查集中元素的数量。初始化时：

- 每个元素的父节点指向其本身，表示每个元素都自成一个集合。
- 每个元素的秩初始化为 `0`。

#### 2.2. **查找（Find）操作**

`find(int x)` 方法用于查找元素 `x` 所在集合的根节点。如果 `x` 不是根节点，就递归查找其父节点，并在此过程中进行**路径压缩**，即将路径上所有的节点直接指向根节点，从而加速后续的查找操作。

#### 2.3. **合并（Union）操作**

`unionSets(int x, int y)` 方法用于将两个元素 `x` 和 `y` 所在的集合合并。如果 `x` 和 `y` 的根节点不相同，说明它们分别属于两个不同的集合。我们根据它们的秩来决定将哪个集合合并到另一个集合：

- 如果 `x` 的秩大于 `y`，将 `y` 的根节点指向 `x`。
- 如果 `x` 的秩小于 `y`，将 `x` 的根节点指向 `y`。
- 如果 `x` 和 `y` 的秩相同，将 `y` 的根节点指向 `x`，并且将 `x` 的秩加1。

#### 2.4. **路径压缩**

在 `find` 操作中，每次递归查找时，我们会将当前节点的父节点指向最终的根节点，这样就能加速后续的查找过程。路径压缩可以使得树的高度保持较小，从而提高并查集的效率。

#### 2.5. **按秩合并**

通过**按秩合并**来控制树的高度。秩表示的是树的深度，合并时将深度小的树挂到深度大的树下面，这样可以防止树变得过高，保持操作的效率。

#### 2.6. **打印并查集状态**

`print()` 方法用于打印并查集中每个元素的父节点和秩（用于调试）。

### 3. 示例输出

```bash
Element 0: Parent = 0, Rank = 0
Element 1: Parent = 2, Rank = 0
Element 2: Parent = 3, Rank = 0
Element 3: Parent = 3, Rank = 1
Element 4: Parent = 5, Rank = 0
Element 5: Parent = 5, Rank = 1
Element 6: Parent = 7, Rank = 0
Element 7: Parent = 7, Rank = 1
Element 8: Parent = 9, Rank = 0
Element 9: Parent = 9, Rank = 1

Find(1): 3
Find(5): 5
Element 0: Parent = 0, Rank = 0
Element 1: Parent = 3, Rank = 0
Element 2: Parent = 3, Rank = 0
Element 3: Parent = 3, Rank = 1
Element 4: Parent = 5, Rank = 0
Element 5: Parent = 5, Rank = 1
Element 6: Parent = 7, Rank = 0
Element 7: Parent = 7, Rank = 1
Element 8: Parent = 9, Rank = 0
Element 9: Parent = 9, Rank = 1
```

### 4. 时间复杂度

- **查找操作** `find(x)`：由于路径压缩，查找操作的时间复杂度接近常数时间，通常认为是 **O(α(n))**，其中 α 是阿克曼函数的逆，增长速度极慢，几乎是常数时间。
- **合并操作** `unionSets(x, y)`：由于按秩合并，合并操作的时间复杂度也是 **O(α(n))**。

由于路径压缩和按秩合并的优化，Union-Find 数据结构在处理大量集合时非常高效。

### 总结

并查集是解决集合合并和查询问题的强大工具，通过路径压缩和按秩合并优化，可以使得并查集操作在大多数情况下接近常数时间，非常适合用来解决图的连通性问题、最小生成树问题等。


# 算法 - 并查集

## 前言

用于解决动态连通性问题，能动态连接两个点，并且判断两个点是否连通。

<div align="center"> <img src="https://cs-notes-1256109796.cos.ap-guangzhou.myqcloud.com/02943a90-7dd4-4e9a-9325-f8217d3cc54d.jpg" width="350"/> </div><br>

| 方法 | 描述 |
| :---: | :---: |
| UF(int N) | 构造一个大小为 N 的并查集 |
| void union(int p, int q) | 连接 p 和 q 节点 |
| int find(int p) | 查找 p 所在的连通分量编号 |
| boolean connected(int p, int q) | 判断 p 和 q 节点是否连通 |

```java
public abstract class UF {

    protected int[] id;

    public UF(int N) {
        id = new int[N];
        for (int i = 0; i < N; i++) {
            id[i] = i;
        }
    }

    public boolean connected(int p, int q) {
        return find(p) == find(q);
    }

    public abstract int find(int p);

    public abstract void union(int p, int q);
}
```

## Quick Find

可以快速进行 find 操作，也就是可以快速判断两个节点是否连通。

需要保证同一连通分量的所有节点的 id 值相等，就可以通过判断两个节点的 id 值是否相等从而判断其连通性。

但是 union 操作代价却很高，需要将其中一个连通分量中的所有节点 id 值都修改为另一个节点的 id 值。

<div align="center"> <img src="https://cs-notes-1256109796.cos.ap-guangzhou.myqcloud.com/0972501d-f854-4d26-8fce-babb27c267f6.jpg" width="320"/> </div><br>

```java
public class QuickFindUF extends UF {

    public QuickFindUF(int N) {
        super(N);
    }


    @Override
    public int find(int p) {
        return id[p];
    }


    @Override
    public void union(int p, int q) {
        int pID = find(p);
        int qID = find(q);

        if (pID == qID) {
            return;
        }

        for (int i = 0; i < id.length; i++) {
            if (id[i] == pID) {
                id[i] = qID;
            }
        }
    }
}
```

## Quick Union

可以快速进行 union 操作，只需要修改一个节点的 id 值即可。

但是 find 操作开销很大，因为同一个连通分量的节点 id 值不同，id 值只是用来指向另一个节点。因此需要一直向上查找操作，直到找到最上层的节点。

<div align="center"> <img src="https://cs-notes-1256109796.cos.ap-guangzhou.myqcloud.com/11b27de5-5a9d-45e4-95cc-417fa3ad1d38.jpg" width="280"/> </div><br>

```java
public class QuickUnionUF extends UF {

    public QuickUnionUF(int N) {
        super(N);
    }


    @Override
    public int find(int p) {
        while (p != id[p]) {
            p = id[p];
        }
        return p;
    }


    @Override
    public void union(int p, int q) {
        int pRoot = find(p);
        int qRoot = find(q);

        if (pRoot != qRoot) {
            id[pRoot] = qRoot;
        }
    }
}
```

这种方法可以快速进行 union 操作，但是 find 操作和树高成正比，最坏的情况下树的高度为节点的数目。

<div align="center"> <img src="https://cs-notes-1256109796.cos.ap-guangzhou.myqcloud.com/23e4462b-263f-4d15-8805-529e0ca7a4d1.jpg" width="100"/> </div><br>

## 加权 Quick Union

为了解决 quick-union 的树通常会很高的问题，加权 quick-union 在 union 操作时会让较小的树连接较大的树上面。

理论研究证明，加权 quick-union 算法构造的树深度最多不超过 logN。

<div align="center"> <img src="https://cs-notes-1256109796.cos.ap-guangzhou.myqcloud.com/a9f18f8a-c1ea-422e-aa56-d91716b0f755.jpg" width="150"/> </div><br>

```java
public class WeightedQuickUnionUF extends UF {

    // 保存节点的数量信息
    private int[] sz;


    public WeightedQuickUnionUF(int N) {
        super(N);
        this.sz = new int[N];
        for (int i = 0; i < N; i++) {
            this.sz[i] = 1;
        }
    }


    @Override
    public int find(int p) {
        while (p != id[p]) {
            p = id[p];
        }
        return p;
    }


    @Override
    public void union(int p, int q) {

        int i = find(p);
        int j = find(q);

        if (i == j) return;

        if (sz[i] < sz[j]) {
            id[i] = j;
            sz[j] += sz[i];
        } else {
            id[j] = i;
            sz[i] += sz[j];
        }
    }
}
```

## 路径压缩的加权 Quick Union

在检查节点的同时将它们直接链接到根节点，只需要在 find 中添加一个循环即可。

## 比较

| 算法 | union | find |
| :---: | :---: | :---: |
| Quick Find | N | 1 |
| Quick Union | 树高 | 树高 |
| 加权 Quick Union | logN | logN |
| 路径压缩的加权 Quick Union | 非常接近 1 | 非常接近 1 |
