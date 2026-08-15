"""
04_rag_pipeline.py
=====================================
完整的 RAG 管道：加载文档 → 切分 → 向量化 → 检索 → 生成。

这是"向量数据库"章节的核心，流程如下：

    知识文档（.txt）
        │  1. TextLoader 加载
        ▼
    原始文本
        │  2. RecursiveCharacterTextSplitter 切块
        ▼
    小文本块
        │  3. 嵌入模型转向量 + Chroma 存储
        ▼
    向量数据库
        │  4. 用户提问 → 语义检索出相关片段
        ▼
    上下文（context）
        │  5. 拼进 Prompt → 大模型
        ▼
    有依据的回答

运行：
    python 02_vector_db/04_rag_pipeline.py
"""

import os
import shutil

from dotenv import load_dotenv
from langchain_chroma import Chroma
from langchain_classic.chains import create_retrieval_chain
from langchain_classic.chains.combine_documents import create_stuff_documents_chain
from langchain_community.document_loaders import TextLoader
from langchain_core.prompts import ChatPromptTemplate
from langchain_openai import ChatOpenAI, OpenAIEmbeddings
from langchain_text_splitters import RecursiveCharacterTextSplitter

load_dotenv()

llm = ChatOpenAI(model=os.getenv("LLM_MODEL", "gpt-4o-mini"), temperature=0)
embeddings = OpenAIEmbeddings(model=os.getenv("EMBEDDING_MODEL", "text-embedding-3-small"))

DATA_FILE = "./02_vector_db/data/langchain_intro.txt"
PERSIST_DIR = "./02_vector_db/chroma_rag_db"


def build_vectorstore() -> Chroma:
    """三步构建向量库：加载 → 切分 → 向量化存储。"""
    # 1. 加载文档
    loader = TextLoader(DATA_FILE, encoding="utf-8")
    documents = loader.load()
    print(f"加载原始文档: {len(documents)} 个文件")

    # 2. 切分：chunk_size 是每块的最大字符数，overlap 让相邻块有重叠，避免上下文被切断
    splitter = RecursiveCharacterTextSplitter(
        chunk_size=200,
        chunk_overlap=50,
        separators=["\n\n", "\n", "。", "，", " "],  # 按语义边界优先切分
    )
    chunks = splitter.split_documents(documents)
    print(f"切分成 {len(chunks)} 个文本块")

    # 3. 向量化并存储
    if os.path.exists(PERSIST_DIR):
        shutil.rmtree(PERSIST_DIR)  # 清掉旧数据，保证可重复执行
    vectorstore = Chroma.from_documents(
        documents=chunks,
        embedding=embeddings,
        collection_name="rag_demo",
        persist_directory=PERSIST_DIR,
    )
    print(f"向量库构建完成，共 {vectorstore._collection.count()} 条向量\n")
    return vectorstore


def build_rag_chain(vectorstore: Chroma):
    """用官方封装好的两条链搭建 RAG：检索链 + 生成链。"""
    # 检索器：对向量库做一层封装，自动把问题转向量并检索
    retriever = vectorstore.as_retriever(search_kwargs={"k": 3})

    # 生成链：把检索到的文档"塞进"提示词，交给模型回答
    prompt = ChatPromptTemplate.from_messages(
        [
            (
                "system",
                "你是一个知识问答助手。请只根据提供的资料回答问题，"
                "资料中没有的信息就说不知道。\n\n资料：\n{context}",
            ),
            ("human", "{input}"),
        ]
    )
    combine_docs_chain = create_stuff_documents_chain(llm, prompt)

    # 检索链：先检索相关文档，再把问题 + 文档交给生成链
    return create_retrieval_chain(retriever, combine_docs_chain)


def ask(rag_chain, question: str) -> None:
    answer = rag_chain.invoke({"input": question})
    print(f"问题: {question}")
    print(f"回答: {answer['answer']}")
    print(f"依据的文档片段: {len(answer['context'])} 条")
    for doc in answer["context"]:
        print(f"  - {doc.page_content[:60]}...")
    print()


if __name__ == "__main__":
    vectorstore = build_vectorstore()
    rag_chain = build_rag_chain(vectorstore)

    # 测试：问题虽然和原文措辞不同，但语义检索能找到相关资料
    ask(rag_chain, "为什么说 RAG 能减少大模型编造答案？")
    ask(rag_chain, "智能体是什么？它和普通问答有什么区别？")
