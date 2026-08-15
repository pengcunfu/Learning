"""
01_connect_and_call_tools.py
=====================================
手动连接 MCP 服务器，把 MCP 工具转换成 LangChain 工具并直接调用。

这个示例不需要 API Key，也不涉及大模型，适合先搞懂"MCP 本身在做什么"：

    LangChain 客户端
        │ 1. stdio_client 启动 math_server.py 子进程
        ▼
    MCP 会话（ClientSession）
        │ 2. load_mcp_tools 获取工具列表（自动根据类型注解生成 JSON Schema）
        ▼
    LangChain 工具（StructuredTool）
        │ 3. tools["add"].invoke({"a": 2, "b": 3})
        ▼
    服务器执行并返回结果

运行：
    python 03_mcp/01_connect_and_call_tools.py
"""

import asyncio
import sys
from pathlib import Path

from langchain_mcp_adapters.tools import load_mcp_tools
from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client

# 计算 math_server.py 的绝对路径（Windows 上必须用绝对路径启动子进程）
SERVER_PATH = Path(__file__).resolve().parent / "servers" / "math_server.py"


def show_text(result) -> str:
    """MCP 工具返回的是内容块列表，这里只提取文本部分方便阅读。"""
    if isinstance(result, list) and result and "text" in result[0]:
        return result[0]["text"]
    return str(result)


async def main() -> None:
    # 1. 配置 stdio 连接参数：用 python 启动 math_server.py 子进程
    server_params = StdioServerParameters(
        # sys.executable = 当前解释器路径，确保子进程用的是同一个虚拟环境
        command=sys.executable,
        args=[str(SERVER_PATH)],
    )

    # 2. 建立连接：stdio_client 返回读写流，ClientSession 在此基础上建立会话
    async with stdio_client(server_params) as (read, write):
        async with ClientSession(read, write) as session:
            # 初始化握手，之后才能收发请求
            await session.initialize()

            # 3. 把 MCP 工具转换成 LangChain 工具（返回的是一个工具列表）
            tools = await load_mcp_tools(session)
            tools_by_name = {tool.name: tool for tool in tools}
            print(f"从 MCP 服务器拿到 {len(tools)} 个工具: {list(tools_by_name.keys())}\n")

            # 4. 逐个调用工具（MCP 工具是异步的，用 await）
            result_add = await tools_by_name["add"].ainvoke({"a": 2, "b": 3})
            print(f"add(2, 3)        = {show_text(result_add)}")

            result_mul = await tools_by_name["multiply"].ainvoke({"a": 4, "b": 7})
            print(f"multiply(4, 7)   = {show_text(result_mul)}")

            result_fib = await tools_by_name["fibonacci"].ainvoke({"n": 8})
            print(f"fibonacci(8)     = {show_text(result_fib)}")

            # 5. 查看工具的参数 Schema（模型就是靠它知道该传什么参数）
            print(f"\nadd 工具的输入 Schema: {tools_by_name['add'].args_schema['properties']}")


if __name__ == "__main__":
    asyncio.run(main())
