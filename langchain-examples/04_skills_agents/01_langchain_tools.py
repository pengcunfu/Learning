"""
01_langchain_tools.py
=====================================
工具（Tool）：LangChain 里"技能"的最小单元。

理解 Skills 之前，先理解工具。一个工具 = 一个函数 + 给模型看的说明书：
- 函数体：真正执行的代码（查数据库、调 API、做计算……）
- 函数签名：类型注解，LangChain 会自动转成 JSON Schema
- docstring：告诉模型"这个工具是干什么的、什么时候用、怎么用"

本示例演示：
1. 用 @tool 定义工具
2. 查看工具生成的 Schema（模型看到的就是这个）
3. 手动实现"思考→调用工具→观察→再思考"循环（智能体的雏形）

运行：
    python 04_skills_agents/01_langchain_tools.py
"""

import os
import sys

from dotenv import load_dotenv
from langchain_core.messages import AIMessage, HumanMessage, ToolMessage
from langchain_core.tools import tool
from langchain_openai import ChatOpenAI

load_dotenv()

# Windows 控制台默认 GBK 编码，换成 UTF-8 避免箭头等特殊字符打印报错
sys.stdout.reconfigure(encoding="utf-8")

llm = ChatOpenAI(model=os.getenv("LLM_MODEL", "gpt-4o-mini"), temperature=0)


# ---------- 1. 定义工具 ----------

@tool
def get_current_time(timezone: str = "Asia/Shanghai") -> str:
    """获取指定时区的当前时间。timezone 是 IANA 时区名，例如 Asia/Shanghai、UTC。"""
    from datetime import datetime, timezone as tz

    try:
        # Python 3.9+ 自带 zoneinfo，不需要额外安装
        from zoneinfo import ZoneInfo

        return datetime.now(ZoneInfo(timezone)).strftime("%Y-%m-%d %H:%M:%S %Z")
    except Exception:
        return datetime.now(tz.utc).strftime("%Y-%m-%d %H:%M:%S UTC")


@tool
def celsius_to_fahrenheit(celsius: float) -> float:
    """把摄氏度转换为华氏度：F = C * 9/5 + 32。"""
    return celsius * 9 / 5 + 32


def demo_tool_schema() -> None:
    """查看工具自动生成的 JSON Schema。"""
    print("get_current_time 的 Schema:")
    print(get_current_time.args_schema.model_json_schema()["properties"])
    print()


def demo_manual_agent_loop() -> None:
    """手写一个最小智能体循环，理解 create_agent 背后发生了什么。"""
    tools = [get_current_time, celsius_to_fahrenheit]
    tools_by_name = {t.name: t for t in tools}

    # bind_tools：把工具列表告诉模型，模型就能在回复中要求调用工具
    llm_with_tools = llm.bind_tools(tools)

    messages = [
        HumanMessage(
            content="现在是几点钟？顺便帮我把 25 摄氏度换算成华氏度。"
        )
    ]

    print("开始手动循环（最多 5 轮）:")
    for step in range(5):
        response: AIMessage = llm_with_tools.invoke(messages)
        messages.append(response)

        # 模型没有要求调用工具，说明它已经准备直接回答了
        if not response.tool_calls:
            print(f"  最终回答: {response.content}\n")
            return

        # 模型要求调用工具：逐个执行，并把结果作为 ToolMessage 放回对话
        for call in response.tool_calls:
            print(f"  第 {step + 1} 轮 → 调用 {call['name']}({call['args']})")
            result = tools_by_name[call["name"]].invoke(call["args"])
            print(f"            ← 结果: {result}")
            messages.append(ToolMessage(content=str(result), tool_call_id=call["id"]))

    print("达到最大轮数，循环结束。")


if __name__ == "__main__":
    demo_tool_schema()
    print("-" * 50)
    demo_manual_agent_loop()
