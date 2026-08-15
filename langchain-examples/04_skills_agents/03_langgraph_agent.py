"""
03_langgraph_agent.py
=====================================
用 LangGraph 手写智能体：看懂"智能体循环"的本质。

create_agent 其实是一个内置好的 LangGraph 图。本示例把它拆开，
用最少的代码展示核心结构：

    START ──→ call_model（模型：决定回答还是调用工具）
                    │
                    ├─ 没有工具调用 → END（直接回答）
                    ▼
              tools（ToolNode：执行工具）
                    │
                    └────→ 回到 call_model（观察结果，继续思考）

额外演示：
- MemorySaver：把对话历史存进检查点，实现多轮记忆
- 每个用户会话（thread_id）有独立的记忆

运行：
    python 04_skills_agents/03_langgraph_agent.py
"""

import os

from dotenv import load_dotenv
from langchain_core.tools import tool
from langchain_openai import ChatOpenAI
from langgraph.checkpoint.memory import MemorySaver
from langgraph.graph import START, MessagesState, StateGraph
from langgraph.prebuilt import ToolNode, tools_condition

load_dotenv()

llm = ChatOpenAI(model=os.getenv("LLM_MODEL", "gpt-4o-mini"), temperature=0)


# ---------- 工具 ----------

@tool
def multiply(a: int, b: int) -> int:
    """将两个数相乘。"""
    return a * b


@tool
def get_city_population(city: str) -> int:
    """返回中国主要城市的人口（万人，教学用模拟数据）。"""
    populations = {"北京": 2189, "上海": 2487, "广州": 1881, "深圳": 1768}
    return populations.get(city, 0)


tools = [multiply, get_city_population]


# ---------- 组装 LangGraph ----------

def call_model(state: MessagesState) -> dict:
    """节点 1：把当前对话历史交给模型。模型可能直接回答，也可能要求调用工具。"""
    response = llm.bind_tools(tools).invoke(state["messages"])
    return {"messages": [response]}


def build_graph():
    graph = StateGraph(MessagesState)

    # 注册两个节点
    graph.add_node("call_model", call_model)
    graph.add_node("tools", ToolNode(tools))

    # 连接：START 进入模型
    graph.add_edge(START, "call_model")

    # 条件边：模型说"要调工具" → 进 tools；否则 → 结束
    graph.add_conditional_edges("call_model", tools_condition)

    # 工具执行完，把结果放回对话，再让模型继续思考
    graph.add_edge("tools", "call_model")

    # 用 MemorySaver 作为检查点存储器，开启多轮记忆
    return graph.compile(checkpointer=MemorySaver())


def chat_loop(agent) -> None:
    print("多轮对话开始（输入 exit 退出）\n")
    thread_id = "user-001"  # 同一个 thread_id 共享记忆
    config = {"configurable": {"thread_id": thread_id}}

    while True:
        user_input = input("你: ").strip()
        if user_input.lower() in ("exit", "quit"):
            break

        result = agent.invoke(
            {"messages": [{"role": "user", "content": user_input}]},
            config=config,
        )
        print(f"AI: {result['messages'][-1].content}\n")


if __name__ == "__main__":
    agent = build_graph()
    chat_loop(agent)
