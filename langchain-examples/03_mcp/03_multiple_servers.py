"""
03_multiple_servers.py
=====================================
MultiServerMCPClient：一次连接多个 MCP 服务器，统一管理工具。

这是实际项目中最常用的方式——智能体同时拥有"数学计算"和"天气查询"
等多组工具，然后按需调用。

运行前：
    1. 另开一个终端启动天气服务器（HTTP）：
       python 03_mcp/servers/weather_server.py
    2. 本示例再运行：
       python 03_mcp/03_multiple_servers.py

不需要 API Key，直接手动调用各服务器的工具。
"""

import asyncio
import sys
from pathlib import Path

from langchain_mcp_adapters.client import MultiServerMCPClient

MATH_SERVER_PATH = Path(__file__).resolve().parent / "servers" / "math_server.py"


def show_text(result) -> str:
    """MCP 工具返回内容块列表，这里只提取文本部分方便阅读。"""
    if isinstance(result, list) and result and "text" in result[0]:
        return result[0]["text"]
    return str(result)


async def main() -> None:
    # 用字典配置多个服务器：key 是服务器名，value 是连接参数
    client = MultiServerMCPClient(
        {
            "math": {  # 本地 stdio 服务器：启动子进程通信
                "transport": "stdio",
                "command": sys.executable,
                "args": [str(MATH_SERVER_PATH)],
            },
            "weather": {  # 远程 HTTP 服务器：通过 URL 通信
                "transport": "http",
                "url": "http://127.0.0.1:8000/mcp",
            },
        }
    )

    # 一次性拿到所有服务器的工具（合并成一个列表，用名字索引更方便）
    tools = await client.get_tools()
    tools_by_name = {tool.name: tool for tool in tools}
    print(f"共拿到 {len(tools)} 个工具: {list(tools_by_name.keys())}\n")

    # 手动调用不同服务器上的工具
    print("math.add(6, 9)      =", show_text(await tools_by_name["add"].ainvoke({"a": 6, "b": 9})))
    print("weather.get_weather =", show_text(await tools_by_name["get_weather"].ainvoke({"city": "北京"})))

    # 注意：MultiServerMCPClient 每次工具调用都会创建独立会话、用完自动清理，
    # 所以这里不需要（也没有）显式关闭操作，程序退出即释放资源。


if __name__ == "__main__":
    asyncio.run(main())
