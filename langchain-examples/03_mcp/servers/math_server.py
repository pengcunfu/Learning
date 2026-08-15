"""
math_server.py
=====================================
一个基于 FastMCP 的 MCP 服务器（stdio 传输），提供数学计算工具。

MCP 服务器有三大核心原语：
- Tools（工具）：模型可以主动调用的函数
- Resources（资源）：可供客户端读取的数据（相当于只读文件）
- Prompts（提示词）：服务端预定义的可复用提示词模板

运行（不需要 API Key）：
    python 03_mcp/servers/math_server.py

注意：stdio 服务器本身不打印日志，运行后保持"等待客户端连接"的状态，
请配合 01_connect_and_call_tools.py 等客户端一起使用。
"""

import logging

import fastmcp.settings as fastmcp_settings
from fastmcp import FastMCP

# 创建 MCP 服务器，名字会显示在客户端工具列表里
mcp = FastMCP("Math")


# ---------- 原语一：Tools（工具） ----------

@mcp.tool()
def add(a: int, b: int) -> int:
    """Add two integers."""
    return a + b


@mcp.tool()
def multiply(a: int, b: int) -> int:
    """Multiply two integers."""
    return a * b


@mcp.tool()
def fibonacci(n: int) -> list[int]:
    """Return the first n Fibonacci numbers."""
    seq = [0, 1]
    while len(seq) < n:
        seq.append(seq[-1] + seq[-2])
    return seq[:n]


# ---------- 原语二：Resources（资源） ----------

@mcp.resource("math://constants")
def get_constants() -> str:
    """暴露一组数学常量（相当于一个只读文件，客户端可主动读取）。"""
    return "pi=3.14159\ne=2.71828\nsqrt2=1.41421"


# ---------- 原语三：Prompts（提示词） ----------

@mcp.prompt()
def explain_formula(formula: str) -> str:
    """给客户端一个预设好的提示词模板，比如让模型解释某个公式。"""
    return f"请用通俗易懂的语言，向初学者解释公式 {formula}，并给出一个生活中的例子。"


if __name__ == "__main__":
    # stdio：客户端把本文件当子进程启动，通过标准输入输出通信
    # show_banner=False 去掉启动横幅；日志级别调高避免刷屏（教学演示更清爽）
    fastmcp_settings.log_enabled = False
    logging.getLogger("fastmcp").setLevel(logging.ERROR)
    mcp.run(transport="stdio", show_banner=False)
