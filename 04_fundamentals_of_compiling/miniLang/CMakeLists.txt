# MiniLang —— 简易编程语言编译器（C++17）
CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(MiniLang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mini
    miniLang/token.h
    miniLang/lexer.h      miniLang/lexer.cpp
    miniLang/ast.h
    miniLang/parser.h     miniLang/parser.cpp
    miniLang/ir.h         miniLang/ir.cpp
    miniLang/interpreter.h miniLang/interpreter.cpp
    miniLang/main.cpp
)

# 可直接编译：cmake -B build && cmake --build build
# 或用一条命令： g++ -std=c++17 miniLang/*.cpp -o mini