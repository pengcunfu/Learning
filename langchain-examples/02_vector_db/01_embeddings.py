"""
01_embeddings.py
=====================================
嵌入（Embedding）：把文字变成向量，让机器能计算"语义距离"。

为什么要向量？
-------------
大模型和数据库都不擅长直接比较两段文字是否"意思相近"。
嵌入模型会把每段文字映射到高维空间的一个点，
意思相近的文字，它们的向量距离也更近。

本示例演示：
1. 把多段文字转成向量
2. 不借助任何框架，手动计算余弦相似度
3. 直观感受"语义相似"与"字面相似"的区别

运行：
    python 02_vector_db/01_embeddings.py
"""

import math
import os

from dotenv import load_dotenv
from langchain_openai import OpenAIEmbeddings

load_dotenv()

# text-embedding-3-small：便宜好用，适合入门
embeddings = OpenAIEmbeddings(model=os.getenv("EMBEDDING_MODEL", "text-embedding-3-small"))


def cosine_similarity(a: list[float], b: list[float]) -> float:
    """余弦相似度：计算两个向量夹角的余弦值，范围 [-1, 1]，越接近 1 越相似。"""
    dot = sum(x * y for x, y in zip(a, b))
    norm_a = math.sqrt(sum(x * x for x in a))
    norm_b = math.sqrt(sum(x * x for x in b))
    return dot / (norm_a * norm_b + 1e-9)  # 加极小值防止除零


def demo_embed_and_similarity() -> None:
    texts = [
        "我今天养了一只可爱的猫",
        "昨天收养了一只橘色的小猫咪",
        "今天天气很好，适合出去跑步",
        "LangChain 是构建大模型应用的框架",
    ]

    # 批量转成向量：embed_documents 返回 list[list[float]]
    vectors = embeddings.embed_documents(texts)
    print(f"每个向量的维度: {len(vectors[0])}\n")

    # 把第一句和其余句子两两比较
    query = "我家新来了一只猫咪"
    query_vec = embeddings.embed_query(query)

    print(f"问题: 「{query}」")
    print("-" * 55)
    for text, vec in zip(texts, vectors):
        score = cosine_similarity(query_vec, vec)
        print(f"相似度 {score:.4f} | {text}")

    print("\n结论：即使字面上没有共同词汇，语义相近的句子得分也更高。")


if __name__ == "__main__":
    demo_embed_and_similarity()
