"""
03_lcel_chain.py
=====================================
LCEL（LangChain Expression Language）：用 | 符号把组件串成管道。

核心思想
--------
prompt | llm | parser
  模板  →  模型  →  解析器

上一级的输出自动作为下一级的输入，就像 Unix 管道一样。
任何一个可运行对象（Runnable）都能参与串联。

运行：
    python 01_basics/03_lcel_chain.py
"""

import os

from dotenv import load_dotenv
from langchain_core.output_parsers import StrOutputParser
from langchain_core.prompts import ChatPromptTemplate
from langchain_core.runnables import RunnableParallel, RunnablePassthrough
from langchain_openai import ChatOpenAI

load_dotenv()

llm = ChatOpenAI(model=os.getenv("LLM_MODEL", "gpt-4o-mini"), temperature=0.7)


def demo_simple_chain() -> None:
    """最基础的 LCEL 链：prompt -> llm -> 字符串解析器。"""
    prompt = ChatPromptTemplate.from_messages(
        [
            ("system", "你是一位{language}诗人。"),
            ("human", "请围绕「{topic}」写一首小诗。"),
        ]
    )

    # StrOutputParser 把 AIMessage 对象转成纯字符串
    chain = prompt | llm | StrOutputParser()

    # 直接 .invoke 一个 dict，管道会自动把字段填充到模板
    poem = chain.invoke({"language": "中文", "topic": "编程"})
    print("小诗:\n", poem)


def demo_parallel() -> None:
    """并行分支：同一输入同时走两条链，最后合并结果。"""
    prompt = ChatPromptTemplate.from_template("用一句话解释：{topic}")

    chain_1 = prompt | llm | StrOutputParser()  # 解释一
    chain_2 = ChatPromptTemplate.from_template("给「{topic}」打个比方：") | llm | StrOutputParser()

    parallel = RunnableParallel(
        解释=chain_1,
        比方=chain_2,
    )

    result = parallel.invoke({"topic": "向量数据库"})
    print("解释:", result["解释"])
    print("比方:", result["比方"])


def demo_rag_shape() -> None:
    """预热一下 RAG 的管道形状：context 由外部检索得到，再拼进提示词。"""
    prompt = ChatPromptTemplate.from_template(
        "根据下面的资料回答问题：\n\n{context}\n\n问题：{question}"
    )

    # RunnablePassthrough 原样透传输入；
    # 这里假设 context 已经由"检索器"填好了（后续 RAG 章节会真正实现）
    chain = (
        RunnablePassthrough.assign(context=lambda _: "LangChain 是一个用于构建大模型应用的框架。")
        | prompt
        | llm
        | StrOutputParser()
    )

    answer = chain.invoke({"question": "LangChain 是什么？"})
    print("RAG 形状的回答:", answer)


if __name__ == "__main__":
    demo_simple_chain()
    print("-" * 50)
    demo_parallel()
    print("-" * 50)
    demo_rag_shape()
