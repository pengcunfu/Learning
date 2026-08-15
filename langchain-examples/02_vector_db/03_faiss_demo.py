"""
03_faiss_demo.py
=====================================
FAISS：Meta 开源的向量检索库，特点是"快"，适合大数据量。
它本身不管理数据，是一个纯内存/文件的索引库。

本示例演示：
1. 写入向量并建立索引
2. 保存到本地文件 / 从文件加载
3. 与 Chroma 的用法对比（接口基本一致，方便切换）

运行：
    python 02_vector_db/03_faiss_demo.py
"""

import os
import shutil
import warnings

from dotenv import load_dotenv
from langchain_openai import OpenAIEmbeddings

# langchain-faiss 独立包目前还是空占位，真正可用的实现仍在 langchain_community 里；
# 这里屏蔽它的弃用警告，让示例输出干净（功能不受影响）
warnings.filterwarnings("ignore", message=".*langchain-community.*sunset.*")
from langchain_community.vectorstores import FAISS  # noqa: E402

load_dotenv()

embeddings = OpenAIEmbeddings(model=os.getenv("EMBEDDING_MODEL", "text-embedding-3-small"))
INDEX_DIR = "./02_vector_db/faiss_index"


def demo_faiss() -> None:
    if os.path.exists(INDEX_DIR):
        shutil.rmtree(INDEX_DIR)

    documents = [
        "FAISS 是 Meta 开源的向量检索库，适合大规模数据。",
        "Chroma 轻量易用，适合本地开发和原型验证。",
        "Milvus 是分布式的向量数据库，适合生产环境。",
        "Qdrant 用 Rust 编写，性能优秀且支持过滤。",
    ]

    # 1. 写入并建立索引
    vectorstore = FAISS.from_texts(documents, embedding=embeddings)
    print(f"索引数量: {vectorstore.index.ntotal}")

    # 2. 保存到本地（会生成 .faiss 和 .pkl 两个文件）
    vectorstore.save_local(INDEX_DIR)
    print(f"索引已保存到: {INDEX_DIR}")

    # 3. 从本地加载（allow_dangerous_deserialization 是加载 .pkl 的安全开关）
    loaded = FAISS.load_local(
        INDEX_DIR,
        embeddings,
        allow_dangerous_deserialization=True,
    )

    # 4. 检索
    print("\n--- 检索：「哪个向量库适合生产环境？」 ---")
    for doc in loaded.similarity_search("哪个向量库适合生产环境？", k=2):
        print(f"- {doc.page_content}")

    # 5. 最大边际相关检索（MMR）：在"相关"和"多样性"之间平衡，避免返回重复内容
    print("\n--- MMR 检索 ---")
    for doc in loaded.max_marginal_relevance_search("向量数据库", k=2):
        print(f"- {doc.page_content}")


if __name__ == "__main__":
    demo_faiss()
