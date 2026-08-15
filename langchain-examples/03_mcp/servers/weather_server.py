"""
weather_server.py
=====================================
一个基于 FastMCP 的 HTTP 服务器（streamable-http），提供模拟天气查询。

与 stdio 不同，HTTP 服务器是独立运行的常驻服务，客户端通过网络访问：

先启动服务器（另开一个终端）：
    python 03_mcp/servers/weather_server.py

启动成功后访问 http://127.0.0.1:8000/mcp 可以看到服务信息。
之后运行 03_multiple_servers.py 或 04_mcp_agent.py 来连接它。
"""

import logging

import fastmcp.settings as fastmcp_settings
from fastmcp import FastMCP

# 服务器名字；host/port 在 run() 时指定（FastMCP 3.x 的写法）
mcp = FastMCP("Weather")


# 模拟的城市天气数据（真实项目中这里通常换成天气 API）
WEATHER_DATA = {
    "北京": "晴，28℃，空气优",
    "上海": "多云，31℃，微风",
    "广州": "雷阵雨，30℃，湿度高",
    "深圳": "阵雨，29℃，适合带伞",
}


@mcp.tool()
def get_weather(city: str) -> str:
    """Get the current weather of a Chinese city."""
    return WEATHER_DATA.get(city, f"抱歉，暂无 {city} 的天气数据，可查询：{'、'.join(WEATHER_DATA)}")


if __name__ == "__main__":
    fastmcp_settings.log_enabled = False
    logging.getLogger("fastmcp").setLevel(logging.ERROR)
    # 客户端将通过 http://127.0.0.1:8000/mcp 访问
    mcp.run(
        transport="streamable-http",
        host="127.0.0.1",
        port=8000,
        show_banner=False,
    )
