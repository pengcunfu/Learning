"""
02_resources_and_prompts.py
=====================================
读取 MCP 服务器的另外两种原语：Resources（资源）和 Prompts（提示词）。

工具是"模型主动调用"，而资源和提示词是"客户端主动读取"：
- 资源：一段数据（文本/文件），相当于服务端暴露的只读文件
- 提示词：服务端预设的提示词模板，客户端可以拉取后直接使用

运行（不需要 API Key）：
    python 03_mcp/02_resources_and_prompts.py
"""

import asyncio
import sys
from pathlib import Path

from langchain_mcp_adapters.prompts import load_mcp_prompt
from langchain_mcp_adapters.resources import load_mcp_resources
from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client

# Windows 控制台默认 GBK 编码，换成 UTF-8 避免特殊字符（如 ²）打印报错
sys.stdout.reconfigure(encoding="utf-8")

SERVER_PATH = Path(__file__).resolve().parent / "servers" / "math_server.py"


async def main() -> None:
    server_params = StdioServerParameters(command=sys.executable, args=[str(SERVER_PATH)])

    async with stdio_client(server_params) as (read, write):
        async with ClientSession(read, write) as session:
            await session.initialize()

            # ---- Resources：读取服务器暴露的只读数据 ----
            blobs = await load_mcp_resources(session)
            print(f"发现 {len(blobs)} 个资源")
            for blob in blobs:
                print(f"  URI: {blob.metadata['uri']}")
                print(f"  内容: {blob.as_string()!r}")

            # ---- Prompts：拉取服务端预设的提示词模板 ----
            messages = await load_mcp_prompt(session, "explain_formula", arguments={"formula": "E=mc²"})
            print(f"\n获取到的提示词模板:")
            for message in messages:
                print(f"  [{message.type}] {message.content}")


if __name__ == "__main__":
    asyncio.run(main())
