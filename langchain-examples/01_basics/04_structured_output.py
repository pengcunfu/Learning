"""
04_structured_output.py
=====================================
结构化输出：让模型按 JSON Schema 返回数据，而不是一段自由文本。

典型场景
--------
- 从用户输入中抽取实体、意图
- 让模型返回固定字段，直接喂给下游程序（如数据库、报表）
- 让 Agent 的中间结果可被程序解析

with_structured_output 会：
1. 根据 Pydantic 模型自动生成 JSON Schema
2. 让模型严格按 Schema 输出（OpenAI 的 response_format / tool 机制）
3. 自动把结果解析成 Pydantic 对象

运行：
    python 01_basics/04_structured_output.py
"""

import os
from typing import Literal

from dotenv import load_dotenv
from langchain_core.prompts import ChatPromptTemplate
from langchain_openai import ChatOpenAI
from pydantic import BaseModel, Field

load_dotenv()

llm = ChatOpenAI(model=os.getenv("LLM_MODEL", "gpt-4o-mini"), temperature=0)


# 1. 定义输出结构（Pydantic 模型）
class MovieReview(BaseModel):
    """一部电影的点评。"""

    title: str = Field(description="电影名称")
    rating: float = Field(description="评分，0 到 10 分")
    sentiment: Literal["positive", "neutral", "negative"] = Field(description="整体情感倾向")
    one_line_summary: str = Field(description="一句话概括")


def demo_structured_output() -> None:
    prompt = ChatPromptTemplate.from_messages(
        [
            ("system", "你是一个影评分析助手，请认真阅读评论并提取信息。"),
            ("human", "请分析这条评论：{review}"),
        ]
    )

    # 2. 绑定输出结构，组成管道
    structured_llm = llm.with_structured_output(MovieReview)
    chain = prompt | structured_llm

    review = "《星际穿越》的配乐和画面太震撼了，剧情虽然有点烧脑，但看完让人回味很久。"
    result: MovieReview = chain.invoke({"review": review})

    # 3. result 是 Pydantic 对象，可以直接访问字段
    print(f"电影: {result.title}")
    print(f"评分: {result.rating}")
    print(f"情感: {result.sentiment}")
    print(f"一句话: {result.one_line_summary}")
    print(f"原始对象: {result.model_dump()}")


if __name__ == "__main__":
    demo_structured_output()
