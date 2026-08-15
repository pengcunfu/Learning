"""
02_prompt_templates.py
=====================================
Prompt 模板：把"提示词的骨架"和"每次变化的输入"分开。

为什么需要模板？
---------------
如果每次都手写完整提示词，代码里会到处拼接字符串，既容易出错又难维护。
ChatPromptTemplate 用占位符 {xxx} 定义骨架，调用时再填充具体内容。

运行：
    python 01_basics/02_prompt_templates.py
"""

import os

from dotenv import load_dotenv
from langchain_core.prompts import (
    ChatPromptTemplate,
    FewShotChatMessagePromptTemplate,
)
from langchain_openai import ChatOpenAI

load_dotenv()

llm = ChatOpenAI(model=os.getenv("LLM_MODEL", "gpt-4o-mini"), temperature=0.3)


def demo_simple_template() -> None:
    """最简单的模板：system + human 两个角色。"""
    prompt = ChatPromptTemplate.from_messages(
        [
            ("system", "你是一位{role}，回答问题时要{style}。"),
            ("human", "请解释：{topic}"),
        ]
    )

    # 用 fill 的方式传入变量（也可以直接 prompt.invoke({...})）
    messages = prompt.format_messages(role="物理老师", style="通俗易懂，多用生活比喻", topic="什么是熵？")
    print("生成的 Prompt:")
    for m in messages:
        print(f"  [{m.type}] {m.content}")

    print("回答:", llm.invoke(messages).content)


def demo_few_shot() -> None:
    """Few-shot：给模型几个示例，让它模仿示例的风格和格式。"""
    examples = [
        {"input": "苹果", "output": "一种红色的水果，也可能是一家科技公司。"},
        {"input": "Python", "output": "一种简单易学的编程语言，以缩进为语法。"},
    ]

    example_prompt = ChatPromptTemplate.from_messages(
        [("human", "{input}"), ("ai", "{output}")]
    )
    few_shot_prompt = FewShotChatMessagePromptTemplate(
        example_prompt=example_prompt,
        examples=examples,
    )

    final_prompt = ChatPromptTemplate.from_messages(
        [
            ("system", "你是百科小助手，请模仿下面的问答风格来回答。"),
            few_shot_prompt,  # 把示例模板嵌进最终模板
            ("human", "{input}"),
        ]
    )

    chain = final_prompt | llm
    print("Few-shot 回答:", chain.invoke({"input": "LangChain"}).content)


if __name__ == "__main__":
    demo_simple_template()
    print("-" * 50)
    demo_few_shot()
