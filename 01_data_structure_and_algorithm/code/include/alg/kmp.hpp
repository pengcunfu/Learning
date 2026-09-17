// kmp.hpp —— KMP 字符串匹配（对应 doc：04_算法设计/10_字符串-KMP算法 与 字符串匹配算法）
//
// 朴素匹配：O(n*m)，每次失配都回退。KMP 核心：预处理 pattern 的 next[]（最长
// 相等前后缀长度），失配时文本指针不回退、只移动模式串，整体 O(n+m)。
#pragma once

#include <string>
#include <vector>

namespace alg {

// 返回 pattern 首次在 text 中出现的位置；找不到返回 -1
int kmpSearch(const std::string& text, const std::string& pat) {
    if (pat.empty()) return 0;
    int m = pat.size();

    // 构造 next 表：next[i] = pat[0..i] 的最长相等真前后缀长度
    std::vector<int> next(m, 0);
    for (int i = 1, j = 0; i < m; ++i) {
        while (j > 0 && pat[i] != pat[j]) j = next[j - 1];
        if (pat[i] == pat[j]) ++j;
        next[i] = j;
    }

    // 在 text 上跑匹配
    for (int i = 0, j = 0; i < (int)text.size(); ++i) {
        while (j > 0 && text[i] != pat[j]) j = next[j - 1];   // 失配 → 回退模式串
        if (text[i] == pat[j]) ++j;
        if (j == m) return i - m + 1;                          // 匹配成功
    }
    return -1;
}

} // namespace alg