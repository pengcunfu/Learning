"""
math_skills 的工具体现。
每个函数都用 @tool 装饰，docstring 就是给模型看的"工具说明书"。
"""

import math

from langchain_core.tools import tool


@tool
def power(base: float, exponent: float) -> float:
    """计算 base 的 exponent 次方，例如 power(2, 10) = 1024。"""
    return base**exponent


@tool
def factorial(n: int) -> int:
    """计算 n 的阶乘（n!），例如 factorial(5) = 120。"""
    return math.factorial(n)


@tool
def gcd(a: int, b: int) -> int:
    """计算两个整数的最大公约数，例如 gcd(48, 18) = 6。"""
    return math.gcd(a, b)
