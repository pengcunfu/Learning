"""
02_chroma_demo.py
=====================================
Chroma：轻量级向量数据库，无需部署服务，一个目录就能持久化。

本示例演示：
1. 把文档写入 Chroma（自动调用嵌入模型转向量）
2. 语义检索 similarity_search
3. 带相似度分数检索 similarity_search_with_score
4. 查看/更新/删除向量

运行：
    python 02_vector_db/02_chroma_demo.py
"""

import os
import shutil

from dotenv import load_dotenv
from langchain_chroma import Chroma
from langchain_openai import OpenAIEmbeddings

load_dotenv()

embeddings = OpenAIEmbeddings(model=os.getenv("EMBEDDING_MODEL", "text-embedding-3-small"))

# 持久化目录（Chroma 会把数据存在本地文件夹里）
PERSIST_DIR = "./02_vector_db/chroma_db"


def demo_chroma() -> None:
    # 每次运行前清理旧数据，保证示例可重复执行
    if os.path.exists(PERSIST_DIR):
        shutil.rmtree(PERSIST_DIR)

    documents = [
        "RAG 是检索增强生成的缩写，能显著降低大模型的幻觉。",
        "向量数据库把文本转换为向量，用余弦相似度做语义搜索。",
        "MCP 是模型上下文协议，让智能体可以调用外部工具。",
        "Chroma 是一个轻量级的向量数据库，适合本地开发。",
        "FAISS 是 Meta 开源的向量检索库，检索速度极快。",
    ]

    # 1. 写入向量库。collection_name 类似"表名"，embedding_function 负责转向量
    vectorstore = Chroma.from_texts(
        texts=documents,
        embedding=embeddings,
        collection_name="demo_collection",
        persist_directory=PERSIST_DIR,
    )
    print(f"已写入 {vectorstore._collection.count()} 条数据，持久化目录: {PERSIST_DIR}")

    # 2. 语义检索：返回最相似的 k 条（默认不返回分数）
    print("\n--- 检索：「怎么降低大模型胡说八道？」 ---")
    results = vectorstore.similarity_search("怎么降低大模型胡说八道？", k=2)
    for i, doc in enumerate(results, 1):
        print(f"{i}. {doc.page_content}")

    # 3. 带分数的检索：分数越小代表距离越近（Chroma 返回的是 L2 距离）
    print("\n--- 带相似度分数检索：「存储向量的数据库」 ---")
    scored = vectorstore.similarity_search_with_score("存储向量的数据库", k=3)
    for doc, score in scored:
        print(f"距离 {score:.4f} | {doc.page_content}")

    # 4. 元数据过滤：Chroma 支持按 metadata 过滤（此处先为文档加上标签）
    #    重新建一个带 metadata 的集合演示过滤
    vectorstore_filter = Chroma.from_texts(
        texts=[
            "如何搭建一个 RAG 系统？",
            "如何把 Chroma 接入 LangChain？",
            "如何用 MCP 连接外部工具？",
        ],
        metadatas=[
            {"category": "rag"},
            {"category": "vector_db"},
            {"category": "mcp"},
        ],
        embedding=embeddings,
        collection_name="filter_collection",
        persist_directory=PERSIST_DIR,
    )
    print("\n--- 元数据过滤：只搜 category=vector_db ---")
    filtered = vectorstore_filter.similarity_search(
        "数据库",
        k=3,
        filter={"category": "vector_db"},
    )
    for doc in filtered:
        print(f"[{doc.metadata['category']}] {doc.page_content}")


if __name__ == "__main__":
    demo_chroma()
