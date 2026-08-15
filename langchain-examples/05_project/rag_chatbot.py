"""
rag_chatbot.py
=====================================
综合项目：RAG 知识问答 + 智能体工具 + 多轮记忆。

把前面学到的三块拼在一起：

    [向量数据库] 知识文档 → 切分 → Chroma → 检索工具
    [Skills/Agents] 检索工具 + 计算工具 → create_agent
    [记忆] MemorySaver 检查点 → 记住多轮对话

可以回答"资料里没有"的问题（用常识）：
    - 向量检索的问题：文档里有答案，智能体去查
    - 数学问题：智能体调用计算工具，保证准确

运行：
    python 05_project/rag_chatbot.py

先构建知识库（约 1 分钟），然后进入多轮对话。
"""

import os
import shutil

from dotenv import load_dotenv
from langchain.agents import create_agent
from langchain_chroma import Chroma
from langchain_community.document_loaders import TextLoader
from langchain_core.tools import tool
from langchain_openai import ChatOpenAI, OpenAIEmbeddings
from langchain_text_splitters import RecursiveCharacterTextSplitter
from langgraph.checkpoint.memory import MemorySaver

load_dotenv()

llm = ChatOpenAI(model=os.getenv("LLM_MODEL", "gpt-4o-mini"), temperature=0.3)
embeddings = OpenAIEmbeddings(model=os.getenv("EMBEDDING_MODEL", "text-embedding-3-small"))

DATA_FILE = "./02_vector_db/data/langchain_intro.txt"
PERSIST_DIR = "./05_project/chroma_chat_db"


# ---------- 1. 构建知识库 ----------

def build_vectorstore() -> Chroma:
    if os.path.exists(PERSIST_DIR):
        shutil.rmtree(PERSIST_DIR)

    loader = TextLoader(DATA_FILE, encoding="utf-8")
    splitter = RecursiveCharacterTextSplitter(chunk_size=200, chunk_overlap=50)
    chunks = splitter.split_documents(loader.load())

    vectorstore = Chroma.from_documents(
        documents=chunks,
        embedding=embeddings,
        collection_name="chatbot_kb",
        persist_directory=PERSIST_DIR,
    )
    print(f"知识库构建完成：{vectorstore._collection.count()} 个片段")
    return vectorstore


# ---------- 2. 把检索能力包装成工具 ----------

def make_retrieval_tool(vectorstore: Chroma):
    retriever = vectorstore.as_retriever(search_kwargs={"k": 3})

    @tool
    def search_knowledge(query: str) -> str:
        """从知识库中检索与问题相关的资料片段。回答知识类问题时必须调用本工具。"""
        docs = retriever.invoke(query)
        return "\n---\n".join(doc.page_content for doc in docs)

    return search_knowledge


# ---------- 3. 另一个普通工具：精确计算 ----------

@tool
def calculate(expression: str) -> float:
    """计算数学表达式，例如 '2 ** 10'、'(3 + 5) * 12'。注意：表达式必须是安全的。"""
    # 教学示例：仅允许数字和运算符号，且禁用内置函数。
    # 生产环境请使用 ast 解析白名单，或改用 numexpr 等专门库，绝不要直接 eval 用户输入。
    allowed = set("0123456789+-*/().% **")
    if any(c not in allowed for c in expression):
        raise ValueError("表达式包含不允许的字符")
    return eval(expression, {"__builtins__": {}}, {})


# ---------- 4. 组装智能体 ----------

def build_agent(vectorstore: Chroma):
    tools = [make_retrieval_tool(vectorstore), calculate]

    return create_agent(
        llm,
        tools,
        system_prompt=(
            "你是一个知识问答助手。"
            "涉及知识类问题时，必须先用 search_knowledge 检索资料再回答，资料不足就直说不知道；"
            "涉及计算时，使用 calculate 工具保证准确。"
        ),
        checkpointer=MemorySaver(),  # 多轮对话记忆
    )


def chat_loop(agent) -> None:
    print("\n知识问答机器人已就绪（输入 exit 退出）\n")
    config = {"configurable": {"thread_id": "learning-demo"}}

    while True:
        question = input("你: ").strip()
        if question.lower() in ("exit", "quit"):
            break

        result = agent.invoke(
            {"messages": [{"role": "user", "content": question}]},
            config=config,
        )
        print(f"AI: {result['messages'][-1].content}\n")


if __name__ == "__main__":
    vectorstore = build_vectorstore()
    agent = build_agent(vectorstore)
    chat_loop(agent)
