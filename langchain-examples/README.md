# LangChain 学习示例

一套从零到一的中文 LangChain 示例，覆盖：**大模型基础调用 → 向量数据库 / RAG → MCP → Skills / 智能体 → 综合项目**。
每个文件都带详细中文注释，建议按顺序阅读和运行。

## 学习路径

| 顺序 | 目录 | 主题 | 你需要掌握的概念 |
| --- | --- | --- | --- |
| 1 | `01_basics` | 大模型基础 | 消息、Prompt 模板、LCEL 管道、结构化输出 |
| 2 | `02_vector_db` | 向量数据库 / RAG | 嵌入、语义检索、Chroma、FAISS、RAG 全流程 |
| 3 | `03_mcp` | MCP | Tools / Resources / Prompts 三大原语、stdio / HTTP 传输 |
| 4 | `04_skills_agents` | Skills / 智能体 | 工具、技能加载、LangGraph 控制流、记忆 |
| 5 | `05_project` | 综合项目 | 把 RAG + 工具 + 记忆拼成一个机器人 |

## 目录结构

```text
langchain-examples/
├── 01_basics/          # 入门：模型调用、提示词、LCEL、结构化输出
├── 02_vector_db/       # 向量数据库：嵌入、Chroma、FAISS、RAG 管道
│   └── data/           # 示例知识文档
├── 03_mcp/             # MCP：FastMCP 服务器 + LangChain 客户端
│   └── servers/        # math（stdio）、weather（HTTP）两个示例服务器
├── 04_skills_agents/   # Skills：工具定义、技能加载器、LangGraph 智能体
│   └── skills/         # 可插拔技能文件夹（SKILL.md + tools.py）
├── 05_project/         # 综合项目：RAG 知识问答机器人
├── requirements.txt
└── .env.example
```

## 环境准备

需要 Python 3.11 或更高版本。

```powershell
# 1. 进入项目目录
cd D:\Learning\langchain-examples

# 2. 创建虚拟环境并安装依赖（建议）
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt

# 3. 配置 API Key：复制模板并填入 key
Copy-Item .env.example .env
notepad .env
```

> 没有 OpenAI Key？也可以填国内兼容服务（DeepSeek / 通义 / 智谱等），在 `.env` 里设置 `OPENAI_BASE_URL` 和 `LLM_MODEL` 即可，示例代码无需改动。

## 各示例速览与运行方法

### 01_basics：大模型基础

```powershell
python 01_basics/01_hello_llm.py      # 最基础的调用 + 流式输出
python 01_basics/02_prompt_templates.py  # 提示词模板、few-shot
python 01_basics/03_lcel_chain.py     # LCEL 管道：prompt | llm | parser
python 01_basics/04_structured_output.py # 让模型按 JSON Schema 输出
```

### 02_vector_db：向量数据库与 RAG

```powershell
python 02_vector_db/01_embeddings.py   # 文字 → 向量，手动计算余弦相似度
python 02_vector_db/02_chroma_demo.py  # Chroma：写入 / 检索 / 过滤
python 02_vector_db/03_faiss_demo.py   # FAISS：本地索引的保存与加载
python 02_vector_db/04_rag_pipeline.py # 完整 RAG：加载→切分→向量化→检索→生成
```

### 03_mcp：模型上下文协议

```powershell
# 不需要 API Key 的示例：
python 03_mcp/01_connect_and_call_tools.py    # 连接 stdio 服务器并直接调用工具
python 03_mcp/02_resources_and_prompts.py     # 读取资源与提示词

# 需要先启动天气服务器（另开一个终端）：
python 03_mcp/servers/weather_server.py

# 然后运行：
python 03_mcp/03_multiple_servers.py   # 一次连接多个服务器
python 03_mcp/04_mcp_agent.py          # 让大模型智能体使用 MCP 工具
```

### 04_skills_agents：Skills 与智能体

```powershell
python 04_skills_agents/01_langchain_tools.py   # 工具 + 手写智能体循环
python 04_skills_agents/02_skill_loader.py      # 动态加载 skills/ 目录下的技能
python 04_skills_agents/03_langgraph_agent.py   # LangGraph 状态图 + 多轮记忆
```

### 05_project：综合项目

```powershell
python 05_project/rag_chatbot.py   # RAG 知识问答 + 工具 + 多轮记忆
```

## 核心概念速查

### 1. 嵌入与向量数据库

- **嵌入（Embedding）**：把文字映射成几百维的向量，语义相近的文字向量距离也近。
- **向量数据库**：存储向量 + 提供近似最近邻检索。示例用 Chroma（轻量、本地文件）和 FAISS（快、适合大规模）。
- **RAG（检索增强生成）**：先从知识库里检索相关资料，再把资料作为上下文交给大模型。好处是回答有据可依、减少幻觉、知识可随时更新（换文档即可）。

### 2. MCP（Model Context Protocol）

MCP 是"模型 ↔ 工具"之间的标准化协议，相当于 AI 世界的 USB-C 接口：

- **服务器（Server）**：暴露能力。示例中 `math_server.py` 暴露计算工具，`weather_server.py` 暴露天气查询。
- **客户端（Client）**：LangChain 通过 `langchain-mcp-adapters` 连接服务器，把 MCP 工具变成 LangChain 工具。
- **传输方式**：`stdio`（本地子进程）和 `streamable-http`（远程服务）。
- **三大原语**：Tools（模型可调用的函数）、Resources（客户端可读取的数据）、Prompts（服务端预设提示词）。

### 3. Skills 与智能体

- **工具（Tool）**：技能的最小单元 = 函数 + 类型注解 + docstring 说明书。
- **技能（Skill）**：一组相关工具 + 使用说明（SKILL.md）。示例中的 `02_skill_loader.py` 实现了一个可插拔技能系统：新增技能 = 新建文件夹。
- **智能体（Agent）**：模型 + 工具 + 循环（思考 → 调用工具 → 观察结果 → 再思考）。`create_agent` 是现成实现；`03_langgraph_agent.py` 用 LangGraph 展示它的内部结构。

## 常见问题

- **报错 `AuthenticationError` / 401**：`.env` 没配置好，检查 `OPENAI_API_KEY`，并确认没把 key 写进代码里。
- **国内网络访问 OpenAI 失败**：换兼容服务，`.env` 中设置 `OPENAI_BASE_URL` 与 `LLM_MODEL`。
- **端口被占用**：改 `weather_server.py` 里的 `port`，同时改客户端里的 `url`。
- **每次运行示例重复生成数据**：示例会自动清理本地的 `chroma_db`、`faiss_index` 等目录，保证可重复执行；它们都在项目内，删除不影响代码。
