// greedy.hpp —— 贪心算法（对应 doc：04_算法设计/01_算法思想/贪心算法 及 07_贪心算法-应用）
//
// 思想：每一步做"当前看起来最优"的局部选择，希望得到全局最优。
// 前提：需证明贪心选择性质与最优子结构才成立。经典实例：
//   - 活动选择：按结束时间最早排序，逐个挑不相冲突的活动，总数最大化。
//   - 找零（进制币）：面额是整倍数时贪心最优。
#pragma once

#include <algorithm>
#include <utility>
#include <vector>

namespace alg {

// 活动选择：intervals 为 {开始,结束}，返回最多能选的不重叠活动数
int activitySelection(std::vector<std::pair<int, int>> intervals) {
    // 先按结束时间升序排序（贪心策略的关键）
    std::sort(intervals.begin(), intervals.end(),
              [](const auto& x, const auto& y) { return x.second < y.second; });
    int count = 0, lastEnd = -1;
    for (auto [s, e] : intervals)
        if (s >= lastEnd) { ++count; lastEnd = e; }   // 能接上就选
    return count;
}

} // namespace alg