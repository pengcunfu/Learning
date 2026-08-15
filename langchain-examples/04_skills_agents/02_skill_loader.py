"""
02_skill_loader.py
=====================================
Skills 加载器：把"技能"做成文件夹，动态加载进智能体。

设计理念（借鉴 Claude Code / Cursor 的 Skills 机制）：

    skills/
    ├── math_skills/
    │   ├── SKILL.md     ← 技能的"使用说明书"（何时用、怎么用、注意事项）
    │   └── tools.py     ← 技能的具体实现（一个个 @tool 函数）
    └── text_skills/
        ├── SKILL.md
        └── tools.py

加载过程：
    1. 遍历 skills/ 目录下的每个子目录
    2. 读取 SKILL.md 作为系统提示词的一部分（让模型知道有这个技能）
    3. 动态导入 tools.py，收集所有 @tool 工具
    4. 把"技能说明 + 工具"交给 create_agent

这样新增一个技能 = 新建一个文件夹，完全不用改主代码，可插拔。

运行：
    python 04_skills_agents/02_skill_loader.py
"""

import importlib.util
import os
from pathlib import Path

from dotenv import load_dotenv
from langchain.agents import create_agent
from langchain_core.tools import BaseTool
from langchain_openai import ChatOpenAI

load_dotenv()

SKILLS_DIR = Path(__file__).resolve().parent / "skills"


def load_skill(skill_dir: Path) -> dict:
    """加载单个技能文件夹，返回 {名称, 说明书, 工具列表}。"""
    # 1. 读取 SKILL.md
    skill_doc = (skill_dir / "SKILL.md").read_text(encoding="utf-8")

    # 2. 动态导入 tools.py（用文件路径加载模块，无需 __init__.py）
    module_name = f"skill_{skill_dir.name}"
    spec = importlib.util.spec_from_file_location(module_name, skill_dir / "tools.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)

    # 3. 收集模块里所有 @tool 装饰的对象
    tools = [obj for obj in vars(module).values() if isinstance(obj, BaseTool)]

    return {
        "name": skill_dir.name,
        "instructions": skill_doc,
        "tools": tools,
    }


def load_all_skills(skills_dir: Path) -> list[dict]:
    """加载 skills/ 下所有技能文件夹。"""
    skills = []
    for entry in sorted(skills_dir.iterdir()):
        if entry.is_dir() and (entry / "SKILL.md").exists() and (entry / "tools.py").exists():
            skills.append(load_skill(entry))
    return skills


def main() -> None:
    llm = ChatOpenAI(model=os.getenv("LLM_MODEL", "gpt-4o-mini"), temperature=0)

    # 加载全部技能
    skills = load_all_skills(SKILLS_DIR)
    all_tools: list[BaseTool] = []
    skill_summaries = []
    for skill in skills:
        all_tools.extend(skill["tools"])
        skill_summaries.append(f"## 技能：{skill['name']}\n{skill['instructions']}")
        print(f"已加载技能 [{skill['name']}]: {[t.name for t in skill['tools']]}")

    # 技能说明书拼进系统提示词，让模型"知道"自己有哪些能力
    system_prompt = (
        "你是一个配备了多项技能的智能助手。\n"
        "下面是你的技能使用说明，遇到对应场景时请使用它们：\n\n"
        + "\n\n".join(skill_summaries)
        + "\n\n回答时请基于工具返回的真实结果。"
    )

    agent = create_agent(llm, all_tools, system_prompt=system_prompt)

    questions = [
        "2 的 10 次方是多少？顺便算一下 5 的阶乘。",
        "分析一下这句话的情绪并提取关键词：这个产品真的棒，我很喜欢，但价格有点坑。",
        "求 48 和 18 的最大公约数。",
    ]

    print("\n" + "=" * 60)
    for question in questions:
        result = agent.invoke({"messages": [{"role": "user", "content": question}]})
        print(f"\n问题: {question}")
        print(f"回答: {result['messages'][-1].content}")


if __name__ == "__main__":
    main()
