"""
01_hello_llm.py
=====================================
最基础的 LangChain 用法：调用大模型，并体验流式输出。

概念速览
--------
- ChatOpenAI: LangChain 对 OpenAI 聊天模型的封装（底层就是 OpenAI 的 Chat Completions API）
- invoke():    同步调用，输入是消息列表，输出是完整的 AI 回复
- stream():    流式调用，逐字返回，适合做打字机效果
- SystemMessage / HumanMessage: 分别对应"系统提示词"和"用户消息"

运行前：
    1. 在项目根目录创建 .env 文件（参考 .env.example）
    2. pip install -r requirements.txt

运行：
    python 01_basics/01_hello_llm.py
"""

import os

from dotenv import load_dotenv
from langchain_core.messages import HumanMessage, SystemMessage
from langchain_openai import ChatOpenAI

# 从 .env 读取 OPENAI_API_KEY 等环境变量
load_dotenv()

# 创建模型实例
# - model: 模型名，可以在 .env 里通过 LLM_MODEL 覆盖
# - temperature: 越低越稳定保守，越高越有创造性
llm = ChatOpenAI(
    model=os.getenv("LLM_MODEL", "gpt-4o-mini"),
    temperature=0.7,
)


def demo_invoke() -> None:
    """最基本的调用：输入消息，得到回答。"""
    messages = [
        SystemMessage(content="你是一位耐心的中文老师，擅长用简单的语言解释复杂概念。"),
        HumanMessage(content="用一句话解释什么是大语言模型？"),
    ]

    # invoke 返回 AIMessage 对象，.content 是文本内容
    response = llm.invoke(messages)
    print("回答:", response.content)


def demo_stream() -> None:
    """流式输出：逐字打印，模拟打字机效果。"""
    messages = [
        HumanMessage(content="请用 3 句话介绍 LangChain，每句话不超过 20 个字。"),
    ]

    print("流式回答: ", end="", flush=True)
    for chunk in llm.stream(messages):
        # stream 返回一个个增量 chunk，累加起来就是完整回答
        print(chunk.content, end="", flush=True)
    print()


if __name__ == "__main__":
    demo_invoke()
    print("-" * 50)
    demo_stream()
