"""
text_skills 的工具体现：纯 Python 实现，不需要额外依赖。
"""

import re
from collections import Counter

from langchain_core.tools import tool

# 简易情绪词典（教学用，真实项目会用模型或现成库）
POSITIVE_WORDS = {"好", "棒", "优秀", "喜欢", "满意", "爱", "厉害", "赞"}
NEGATIVE_WORDS = {"差", "烂", "糟糕", "讨厌", "失望", "垃圾", "坑", "愤怒"}


@tool
def count_words(text: str) -> dict:
    """统计一段文本：总字数（不含空格）、中文词数（按标点切分）、英文单词数。"""
    total_chars = len(re.sub(r"\s", "", text))
    chinese_segments = [s for s in re.split(r"[，。！？、；：,.!?;: ]+", text) if s]
    english_words = re.findall(r"[A-Za-z]+", text)
    return {
        "总字数": total_chars,
        "中文词组数": len(chinese_segments),
        "英文单词数": len(english_words),
    }


@tool
def extract_keywords(text: str, top_k: int = 5) -> list[str]:
    """基于词频从文本中抽取关键词（简化版，只按出现次数排序，不含语义）。"""
    words = re.findall(r"[\u4e00-\u9fff]{2,}|[A-Za-z]{2,}", text)
    counter = Counter(words)
    return [word for word, _ in counter.most_common(top_k)]


@tool
def sentiment_score(text: str) -> str:
    """用简易词典判断文本情绪倾向：返回 positive / neutral / negative 之一。"""
    positive = sum(1 for w in POSITIVE_WORDS if w in text)
    negative = sum(1 for w in NEGATIVE_WORDS if w in text)
    if positive > negative:
        return "positive"
    if negative > positive:
        return "negative"
    return "neutral"
