# cn_ds —— 数据结构与算法学习代码 (C++17 / MinGW / Windows)

cmake_minimum_required(VERSION 3.16)
project(cn_ds LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# header-only 库：把 include/ 暴露给使用方
add_library(dslib INTERFACE)
target_include_directories(dslib INTERFACE include)

# 单测试入口：build 出 ds.exe，运行全部断言自检
add_executable(ds tests/main.cpp)
target_link_libraries(ds PRIVATE dslib)

# 构建（Windows / MinGW）
#   cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++
#   cmake --build build
#   build\ds.exe