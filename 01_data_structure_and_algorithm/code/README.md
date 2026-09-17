# 数据结构与算法 · 学习代码（C++17 / MinGW / Windows）

与 `../docs/` 学习文档配套的一整套可运行代码。每个数据结构/算法一个头文件，
`tests/main.cpp` 用 115 条断言自检全部实现，输出"通过 N / 失败 M"。

## 构建与运行

```bash
cd code
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++
cmake --build build
build\ds.exe        # 运行全部断言
```

要求：CMake + MinGW-w64（含 `g++`、`mingw32-make`），并把 MinGW 的 `bin` 加入 `PATH`。

## 目录：每个文件对应哪个文档

### 基础数据结构 `include/ds/`
| 文件 | 对应 doc | 要点 |
|------|----------|------|
| `dynamic_array.hpp` | 01_基础数据结构/01_数组 | 连续内存、O(1) 访问、自动扩容 |
| `linked_list.hpp` | 05_链表 | 单链表、反转、快慢指针（判环/中点）|
| `stack.hpp` | 03_栈 | 顺序栈；额外最小栈 MinStack |
| `queue.hpp` | 04_队列 | 循环队列 + 链式队列（FIFO）|
| `hash_map.hpp` | 02_哈希表 | 链地址法、负载因子、rehash |
| `hash_set.hpp` | 07_集合 | 去重/成员检测，底层哈希表 |
| `union_find.hpp` | 04_算法设计/查找/并查集 | 路径压缩 + 按秩合并 |

### 树 `include/ds/`
| 文件 | 对应 doc | 要点 |
|------|----------|------|
| `binary_tree.hpp` | 02_树结构/01_树基础 | 前/中/后序、层序(BFS)、树高 |
| `binary_search_tree.hpp` | 02_二叉搜索树BST | 查找/插入/删除/最值 |
| `avl_tree.hpp` | 03_平衡二叉树AVL | LL/RR/LR/RL 旋转、平衡因子、check() |
| `red_black_tree.hpp` | 04_红黑树 | 五条性质、插入/删除修复、check() |
| `binary_heap.hpp` | 07_堆Heap | 最大堆、上浮/下沉、heapify |
| `priority_queue.hpp` | 07_堆Heap 应用 | 大顶/小顶可切换（Top-K、供图算法）|
| `trie.hpp` | 08_Trie树 | 前缀匹配、自动补全 |

### 图与相关算法 `include/alg/`
| 文件 | 对应 doc | 要点 |
|------|----------|------|
| `graph.hpp` | 03_图论/01_图基础 | 邻接表 + BFS/DFS（含迭代 DFS）|
| `dijkstra.hpp` | 03_最短路径 | 单源最短路，堆优化 |
| `floyd.hpp` | 04_多源最短路径 | 动态规划 O(V³) |
| `prim.hpp` | 02_最小生成树-Prim | 加点法，堆优化 |
| `kruskal.hpp` | 09_最小生成树-Kruskal | 加边法 + 并查集判环 |
| `topological_sort.hpp` | 05_拓扑排序 | Kahn（BFS）与 DFS 两种 |
| `tarjan_scc.hpp` | 06_强连通分量 | 一次 DFS 求所有 SCC |

### 算法 `include/alg/`
| 文件 | 对应 doc | 要点 |
|------|----------|------|
| `sorting.hpp` | 04_算法设计/02_排序算法 | 冒泡/选择/插入/希尔/归并/快排/堆 + 计数/基数 |
| `binary_search.hpp` | 03_查找/折半查找 | 基本版 + lower_bound/upper_bound |
| `dynamic_programming.hpp` | 01_算法思想/动态规划 | 斐波那契/LIS/0-1背包/LCS |
| `backtracking.hpp` | 01_算法思想/回溯算法 | N皇后/全排列/组合求和 |
| `greedy.hpp` | 01_算法思想/贪心算法 | 活动选择 |
| `kmp.hpp` | 10_字符串-KMP算法 | 前缀函数 + 线性匹配 |

## 测试 `tests/main.cpp`
- 自研轻量断言，无第三方依赖。每个测试函数内 `sect()` 标注对应 doc 章节。
- 覆盖正常、空、边界、错误路径（未声明删除、不存在元素、除零等）。
- 关键性质自检：AVL/红黑树插入删除后 `check()` 仍平衡/仍满足五条性质。
- 结果尾部：`======== 结果: 通过 115 / 失败 0 ========`（失败 >0 时退出码为 1）。

## 进阶扩展（未包含）
- **B 树 / B+ 树**：偏数据库索引、量大，README 标注为后续方向。
- 一整套 LeetCode / 剑指Offer 题解：属于 `../docs/` 配套题解范畴。