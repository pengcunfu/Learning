"""
04_mcp_agent.py
=====================================
让大模型智能体使用 MCP 工具：这是 MCP 与 LangChain 结合的标准姿势。

流程：
    用户提问 "计算 (3+5)*12，再查一下北京的天气"
        │
        ▼
    create_agent 创建的智能体
        │  自己规划：先算数 → 再查天气（可能多次调用工具）
        ▼
    MultiServerMCPClient 里的 math / weather 工具
        │
        ▼
    汇总成最终回答

运行前：
    1. 配置 .env 里的 OPENAI_API_KEY（或用其它模型）
    2. 另开终端启动天气服务器：
       python 03_mcp/servers/weather_server.py
    3. 运行本示例：
       python 03_mcp/04_mcp_agent.py
"""

import asyncio
import os
import sys
from pathlib import Path

from dotenv import load_dotenv
from langchain.agents import create_agent
from langchain_mcp_adapters.client import MultiServerMCPClient
from langchain_openai import ChatOpenAI

load_dotenv()

# Windows 控制台默认 GBK 编码，换成 UTF-8 避免箭头等特殊字符打印报错
sys.stdout.reconfigure(encoding="utf-8")

MATH_SERVER_PATH = Path(__file__).resolve().parent / "servers" / "math_server.py"


async def main() -> None:
    # 1. 大模型：智能体的"大脑"，负责规划与总结
    llm = ChatOpenAI(model=os.getenv("LLM_MODEL", "gpt-4o-mini"), temperature=0)

    # 2. 连接 MCP 服务器，拿到工具（智能体的"手脚"）
    client = MultiServerMCPClient(
        {
            "math": {
                "transport": "stdio",
                "command": sys.executable,
                "args": [str(MATH_SERVER_PATH)],
            },
            "weather": {
                "transport": "http",
                "url": "http://127.0.0.1:8000/mcp",
            },
        }
    )
    tools = await client.get_tools()

    # 3. 创建智能体：create_agent 会自动给模型绑定工具，
    #    并内置"思考→调用工具→观察结果→再思考"的执行循环
    agent = create_agent(
        llm,
        tools,
        system_prompt="你是一个乐于助人的助理，需要调用工具时请直接调用。",
    )

    # 4. 提问，让智能体自己决定调用哪些工具
    response = await agent.ainvoke(
        {"messages": [{"role": "user", "content": "帮我计算 (3+5)*12 等于多少，顺便查一下北京的天气。"}]}
    )

    print("\n智能体的思考与执行过程：")
    for message in response["messages"]:
        role = getattr(message, "type", "unknown")
        if role == "ai" and message.tool_calls:
            for call in message.tool_calls:
                print(f"  → 决定调用工具: {call['name']}({call['args']})")
        elif role == "tool":
            print(f"  ← 工具返回: {message.content[:80]}")
        elif role == "ai" and not message.tool_calls:
            print(f"  最终回答: {message.content}")

    # MultiServerMCPClient 的每次工具调用都使用独立会话，自动清理，无需显式关闭


if __name__ == "__main__":
    asyncio.run(main())
