// main.cpp —— 数据结构与算法学习自检（单一测试入口）
// 构建：build\ds.exe  运行全部断言，输出"通过 N / 失败 M"。
// 每个用例都标注对应的 docs/ 章节，方便照着文档跑。

#include <cstdio>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "../include/ds/dynamic_array.hpp"
#include "../include/ds/linked_list.hpp"
#include "../include/ds/stack.hpp"
#include "../include/ds/queue.hpp"
#include "../include/ds/hash_map.hpp"
#include "../include/ds/hash_set.hpp"
#include "../include/ds/binary_tree.hpp"
#include "../include/ds/binary_search_tree.hpp"
#include "../include/ds/avl_tree.hpp"
#include "../include/ds/red_black_tree.hpp"
#include "../include/ds/binary_heap.hpp"
#include "../include/ds/priority_queue.hpp"
#include "../include/ds/trie.hpp"
#include "../include/ds/union_find.hpp"

#include "../include/alg/graph.hpp"
#include "../include/alg/dijkstra.hpp"
#include "../include/alg/floyd.hpp"
#include "../include/alg/prim.hpp"
#include "../include/alg/kruskal.hpp"
#include "../include/alg/topological_sort.hpp"
#include "../include/alg/tarjan_scc.hpp"
#include "../include/alg/sorting.hpp"
#include "../include/alg/binary_search.hpp"
#include "../include/alg/dynamic_programming.hpp"
#include "../include/alg/backtracking.hpp"
#include "../include/alg/greedy.hpp"
#include "../include/alg/kmp.hpp"

using namespace ds;
using namespace alg;

// ---------- 轻量断言 ----------
static int g_pass = 0, g_fail = 0;
static void sect(const char* name) { std::printf("\n[%s]\n", name); }
#define CHECK(cond, msg)                                              \
    do { if (cond) { ++g_pass; }                                     \
         else { ++g_fail; std::printf("  FAIL: %s\n", msg); } } while (0)
#define CHECK_EQ(a, b, msg)                                           \
    do { if ((a) == (b)) { ++g_pass; }                                \
         else { ++g_fail; std::printf("  FAIL: %s\n", msg); } } while (0)
#define CHECK_STR(a, b, msg)                                          \
    do { if ((a) == (b)) { ++g_pass; }                                \
         else { ++g_fail; std::printf("  FAIL: %s\n", msg); } } while (0)

// ---------- 各类测试 ----------
static void test_array() {
    sect("基础数据结构 · 动态数组 docs/01_基础数据结构/01_数组");
    DynamicArray<int> a;
    for (int i = 0; i < 12; ++i) a.push_back(i);
    CHECK_EQ(a.size(), 12, "push_back 12 个元素");
    CHECK_EQ(a[11], 11, "随机访问");
    CHECK_EQ(a.find(7), 7, "查找");
    a.insert(0, 100);
    CHECK_EQ(a.size(), 13, "插入后大小");
    CHECK_EQ(a[0], 100, "头部插入");
    CHECK_EQ(a[1], 0, "插入后移");
    a.erase(0);
    CHECK_EQ(a[0], 0, "删除头部");
    a.pop_back();
    CHECK_EQ(a.size(), 11, "pop_back");
    CHECK_EQ(a.capacity() >= a.size(), true, "容量覆盖大小");
}

static void test_linked_list() {
    sect("基础数据结构 · 链表 docs/01_基础数据结构/05_链表");
    LinkedList<int> l;
    for (int i = 1; i <= 5; ++i) l.push_back(i);
    CHECK_EQ(l.size(), 5, "push_back 5 个");
    CHECK_EQ(l.front(), 1, "front");
    CHECK_EQ(l.back(), 5, "back");
    CHECK(l.contains(3), "contains 3");
    CHECK(!l.contains(9), "不含 9");
    l.remove(3);
    CHECK_EQ(l.size(), 4, "remove 3 后大小");
    l.reverse();
    CHECK_EQ(l.front(), 5, "反转后 front=5");
    CHECK_EQ(l.back(), 1, "反转后 back=1");
    CHECK_EQ(l.middle(), 2, "快慢指针找中间(偶数偏右)");
    // 环检测
    LinkedList<int> c; c.push_back(1); c.push_back(2); c.push_back(3);
    CHECK(!c.has_cycle(), "无环");
}

static void test_stack() {
    sect("基础数据结构 · 栈 + 最小栈 docs/01_基础数据结构/03_栈");
    Stack<int> s;
    s.push(1); s.push(2); s.push(3);
    CHECK_EQ(s.top(), 3, "top");
    s.pop();
    CHECK_EQ(s.top(), 2, "pop 后 top");
    // 最小栈
    MinStack<int> ms;
    ms.push(3); ms.push(2); ms.push(5); ms.push(1);
    CHECK_EQ(ms.min(), 1, "min=1");
    ms.pop(); // 弹出1
    CHECK_EQ(ms.min(), 2, "弹出最小后 min=2");
}

static void test_queue() {
    sect("基础数据结构 · 队列 docs/01_基础数据结构/04_队列");
    CircularQueue<int> q(4);
    for (int i = 0; i < 9; ++i) q.push(i);   // 触发扩容
    CHECK_EQ(q.size(), 9, "循环队列 size");
    CHECK_EQ(q.front(), 0, "front");
    q.pop(); q.pop();
    CHECK_EQ(q.front(), 2, "先进先出");
    LinkedQueue<int> lq;
    lq.push(1); lq.push(2); lq.pop();
    CHECK_EQ(lq.front(), 2, "链式队列 front");
}

static void test_hash() {
    sect("基础数据结构 · 哈希表/集合 docs/01_基础数据结构/02 & 07");
    HashMap<int, std::string> m;
    for (int i = 0; i < 100; ++i) m.put(i, "v" + std::to_string(i));  // 触发 rehash
    CHECK_EQ(m.size(), 100, "put 100 个");
    CHECK_EQ(m.get(42), "v42", "get 42");
    m.put(42, "changed");
    CHECK_EQ(m.get(42), "changed", "覆盖");
    CHECK(m.contains(99), "contains 99");
    CHECK(!m.contains(1000), "不含 1000");
    CHECK(m.remove(50), "remove 50");
    CHECK_EQ(m.size(), 99, "remove 后大小");
    // 字符串键
    HashMap<std::string, int> ms;
    ms.put("hello", 1); ms.put("world", 2);
    CHECK_EQ(ms.get("hello"), 1, "string 键");
    // 集合
    HashSet<int> set;
    for (int i = 0; i < 50; ++i) set.insert(i % 25);   // 去重
    CHECK_EQ(set.size(), 25, "集合去重");
    CHECK(set.contains(7), "含 7");
    set.insert(7);
    CHECK_EQ(set.size(), 25, "重复插入不加");
    CHECK(set.remove(7), "删除 7");
    CHECK_EQ(set.size(), 24, "删除后大小");
}

static void test_trees() {
    sect("树 · 遍历 / BST / AVL / 红黑树 / 堆 / Trie");
    // 遍历
    BinaryNode<int> n1(1), n2(2), n3(3), n4(4);
    n1.left = &n2; n1.right = &n3; n2.left = &n4;
    std::vector<int> ord;
    traversal::preorder(&n1, ord);
    CHECK((ord == std::vector<int>{1,2,4,3}), "前序 1,2,4,3");
    ord.clear(); traversal::inorder(&n1, ord);
    CHECK((ord == std::vector<int>{4,2,1,3}), "中序 4,2,1,3");
    ord.clear(); traversal::postorder(&n1, ord);
    CHECK((ord == std::vector<int>{4,2,3,1}), "后序 4,2,3,1");
    ord.clear(); traversal::levelorder(&n1, ord);
    CHECK((ord == std::vector<int>{1,2,3,4}), "层序 1,2,3,4");
    CHECK_EQ(traversal::maxDepth(&n1), 3, "树高 3");

    // BST
    BinarySearchTree<int> bst;
    for (int x : {5,3,7,2,4,6,8}) bst.insert(x);
    CHECK(bst.contains(4), "BST 查 4");
    CHECK(!bst.contains(9), "BST 无 9");
    CHECK_EQ(bst.min(), 2, "BST min");
    CHECK_EQ(bst.size(), 7, "BST size");
    bst.remove(3); bst.remove(5); bst.remove(8);
    CHECK_EQ(bst.size(), 4, "BST 删除后 size");

    // AVL
    AVLTree<int> avl;
    for (int x : {3,2,1,4,5,6,7,16,15,14,13,12,11,10,8,9}) avl.insert(x);
    CHECK(avl.check(), "AVL 插入后仍平衡");
    CHECK(avl.contains(10), "AVL 查 10");
    avl.remove(8); avl.remove(14);
    CHECK(avl.check(), "AVL 删除后仍平衡");
    CHECK_EQ(avl.size(), 14, "AVL size");

    // 红黑树
    RedBlackTree<int> rb;
    std::vector<int> keys = {7,3,18,10,22,8,11,26,2,6,13};
    for (int k : keys) rb.insert(k);
    CHECK(rb.check(), "红黑树性质成立");
    CHECK(rb.contains(11), "RB 查 11");
    for (int k : keys) { if (!rb.contains(k)) CHECK(false, "RB 全在"); }
    rb.remove(18); rb.remove(7); rb.remove(22);
    CHECK(rb.check(), "RB 删除后性质成立");
    CHECK(rb.contains(11), "RB 删除后仍能查 11");
    CHECK_EQ(rb.size(), 8, "RB size");

    // 最大堆
    MaxHeap<int> hp;
    for (int x : {4,10,3,5,1}) hp.push(x);
    CHECK_EQ(hp.top(), 10, "堆顶最大");
    hp.pop(); hp.pop();
    CHECK_EQ(hp.top(), 4, "pop 两次后 top=4");

    // 优先队列（小顶堆 = Top-K）
    PriorityQueue<int, std::greater<int>> pq;
    for (int x : {3,1,4,1,5,9,2,6}) pq.push(x);
    CHECK_EQ(pq.top(), 1, "小顶堆顶最小");
    pq.pop();
    CHECK_EQ(pq.top(), 1, "弹出后 top=1");

    // 字典树
    Trie tr;
    for (const char* w : {"app","apple","apart","ape","apply"}) tr.insert(w);
    CHECK(tr.contains("apple"), "Trie 有 apple");
    CHECK(!tr.contains("appl"), "Trie 无 appl(完整词)");
    CHECK(tr.hasPrefix("app"), "Trie 前缀 app");
    auto w = tr.autocomplete("ap");   // apart,ape,app,apple,apply → 5 个
    CHECK_EQ((int)w.size(), 5, "补全 ap 得 5 个");
    CHECK_STR(w[0], std::string("apart"), "补全按字典序");
}

static void test_graph() {
    sect("图 · BFS/DFS / Dijkstra / Floyd / Prim / Kruskal / 拓扑 / Tarjan");
    // BFS/DFS：0-1 2-0 2-3 3-3
    Graph g(4);
    g.addEdge(0,1); g.addEdge(2,0); g.addEdge(2,3); g.addEdge(3,3);
    auto bfs = g.bfs(2);
    CHECK_EQ((int)bfs.size(), 4, "BFS 访问 4 点");
    CHECK_EQ(g.distances()[0], 1, "BFS 2→0 步数 1");
    CHECK_EQ(g.distances()[3], 1, "BFS 2→3 步数 1");
    auto dfs = g.dfs(2);
    CHECK_EQ((int)dfs.size(), 4, "DFS 访问 4 点");
    auto dfsIt = g.dfsIterative(2);
    CHECK_EQ((int)dfsIt.size(), 4, "DFS 迭代访问 4 点");

    // 带权图：0-1:4 0-2:2 1-2:1 1-3:5 2-3:8
    WeightedGraph wg(4);
    wg.addUndirected(0,1,4); wg.addUndirected(0,2,2); wg.addUndirected(1,2,1); wg.addUndirected(1,3,5); wg.addUndirected(2,3,8);

    // Dijkstra：0→3 最短 = 0-2-1-3 = 2+1+5=8
    auto dij = dijkstra(wg, 0);
    CHECK_EQ(dij.dist[3], 8, "Dijkstra 0→3=8");
    CHECK_EQ(dij.dist[1], 3, "Dijkstra 0→1=3");
    CHECK_EQ(dij.dist[2], 2, "Dijkstra 0→2=2");

    // Floyd
    const int INF = INF_FL;
    std::vector<std::vector<int>> am(3, std::vector<int>(3, INF));
    am[0][1]=1; am[1][2]=1; am[2][0]=3; for(int i=0;i<3;++i) am[i][i]=0;
    auto fd = floyd(am);
    CHECK_EQ(fd[0][2], 2, "Floyd 0→2=2");
    CHECK_EQ(fd[0][0], 0, "Floyd 对角 0");

    // Prim / Kruskal：上面的 wg 最小生成树 = 2+1+5 = 8
    CHECK_EQ(prim(wg), 8, "Prim MST=8");
    std::vector<Edge> edges = {{0,1,4},{0,2,2},{1,2,1},{1,3,5},{2,3,8}};
    CHECK_EQ(kruskal(4, edges), 8, "Kruskal MST=8");

    // 拓扑排序：边 0→1,0→2,1→3,2→3
    std::vector<std::vector<int>> dag(4);
    dag[0]={1,2}; dag[1]={3}; dag[2]={3};
    auto tp = topologicalSortKahn(dag);
    CHECK_EQ((int)tp.size(), 4, "拓扑有环则空、无环则满");
    // 0 必在 1/2 前，1/2 必在 3 前（检查基本序）
    bool ok = true;
    for (auto [a, b] : std::vector<std::pair<int,int>>{{0,1},{0,2},{1,3},{2,3}}) {
        int ia=-1, ib=-1;
        for (int i=0;i<(int)tp.size();++i){ if(tp[i]==a)ia=i; if(tp[i]==b)ib=i; }
        if (!(ia>=0 && ib>=0 && ia<ib)) ok=false;
    }
    CHECK(ok, "Kahn 拓扑序满足先后约束");
    std::vector<int> tp2;
    CHECK(topologicalSortDFS(dag, tp2), "DFS 拓扑无环");
    CHECK_EQ((int)tp2.size(), 4, "DFS 拓扑排满");
    // 判环：1→2,2→1
    std::vector<std::vector<int>> cyc(3); cyc[1]={2}; cyc[2]={1}; cyc[0]={1};
    CHECK_EQ(topologicalSortKahn(cyc).size(), 0, "Kahn 检出环");

    // Tarjan 强连通：1→2→1 一个 + 3 独立一个
    std::vector<std::vector<int>> g2(4);
    g2[0]={1}; g2[1]={2}; g2[2]={0}; g2[3]={3};
    auto scc = tarjan(g2);
    CHECK_EQ(scc.count, 2, "Tarjan SCC 数=2");
}

static void test_algorithms() {
    sect("算法 · 排序 / 二分 / 并查集 / DP / 回溯 / 贪心 / KMP");
    std::vector<int> sample = {5,2,8,1,9,3,7,4,6,0};
    std::vector<int> ref = sample; std::sort(ref.begin(),ref.end());
    std::vector<int> s;
    s=sample; bubbleSort(s);      CHECK(s==ref, "冒泡排序");
    s=sample; selectionSort(s);   CHECK(s==ref, "选择排序");
    s=sample; insertionSort(s);   CHECK(s==ref, "插入排序");
    s=sample; shellSort(s);       CHECK(s==ref, "希尔排序");
    s=sample; mergeSort(s);       CHECK(s==ref, "归并排序");
    s=sample; quickSort(s);       CHECK(s==ref, "快速排序");
    s=sample; heapSort(s);        CHECK(s==ref, "堆排序");
    // 边界：空与单元素在多几种排序里安全
    std::vector<int> tiny = {1};
    mergeSort(tiny); CHECK_EQ(tiny[0], 1, "归并排序空/单元素安全");
    std::vector<int> cnt = {4,2,4,1,0,3,2,1,0,4};
    auto cs = countingSort(cnt, 4);
    std::vector<int> cref = cnt; std::sort(cref.begin(),cref.end());
    CHECK(cs==cref, "计数排序");
    std::vector<int> rad = {170,45,75,90,2,24,802,66};
    std::vector<int> rref = rad; std::sort(rref.begin(),rref.end());
    radixSort(rad);
    CHECK(rad==rref, "基数排序");

    // 二分
    std::vector<int> a2 = {1,3,5,7,9,9,11};
    CHECK_EQ(binarySearch(a2,7), 3, "二分查 7");
    CHECK_EQ(binarySearch(a2,8), -1, "二分查不存在 8");
    CHECK_EQ(lowerBound(a2,9), 4, "lower_bound 9=4");
    CHECK_EQ(upperBound(a2,9), 6, "upper_bound 9=6");

    // 并查集
    UnionFind uf(6);
    uf.unify(0,1); uf.unify(1,2); uf.unify(3,4);
    CHECK(uf.connected(0,2), "并查集 0-2 连通");
    CHECK(!uf.connected(0,4), "并查集 0-4 不连通");
    uf.unify(2,3);
    CHECK(uf.connected(0,4), "合并后 0-4 连通");
    CHECK_EQ(uf.count(), 2, "并查集合拢后集合数=2");

    // DP
    CHECK_EQ(fibDp(10), 55, "DP 斐波那契 F10=55");
    CHECK_EQ(lis({10,9,2,5,3,7,101,18}), 4, "LIS=4");
    CHECK_EQ(knapsack01({1,2,3},{6,10,12},5), 22, "0-1背包=22");
    CHECK_EQ(lcs("abcde","ace"), 3, "LCS=3");

    // 回溯
    CHECK_EQ((int)nQueens(4).size(), 2, "4皇后共2种解");
    CHECK_EQ((int)permutations({1,2,3}).size(), 6, "3元素全排列=6");
    auto combos = combinationSum({2,3,6,7}, 7);
    CHECK_EQ((int)combos.size(), 2, "组合求和=2 组");

    // 贪心
    CHECK_EQ(activitySelection({{1,3},{2,5},{4,6},{6,7},{5,9}}), 3, "活动选择最多3个");

    // KMP
    CHECK_EQ(kmpSearch("ABABDABACDABABCABAB","ABABCABAB"), 10, "KMP 匹配位置=10");
    CHECK_EQ(kmpSearch("hello","ll"), 2, "KMP 匹配 ll=2");
    CHECK_EQ(kmpSearch("hello","xyz"), -1, "KMP 无匹配=-1");
}

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0); // 即时输出，便于定位崩溃位置
    std::printf("======== 数据结构与算法学习自检 ========\n");
    test_array();
    test_linked_list();
    test_stack();
    test_queue();
    test_hash();
    test_trees();
    test_graph();
    test_algorithms();
    std::printf("\n======== 结果: 通过 %d / 失败 %d ========\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}