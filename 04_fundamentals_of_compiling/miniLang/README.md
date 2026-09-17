# MiniLang —— 从零手搓的一门简易编程语言

用约 600 行 C++ 实现的一门微型语言编译器，**覆盖完整编译流程**，专门用来学编译原理。
每个编译阶段都对应一个独立源文件，可以一条命令看到整条管线：字符流 → 记号流 → AST → 三地址中间代码 → 执行结果。

## 语言长什么样

```
// examples/fib.mini：递归求斐波那契
func fib(n) {
    if (n <= 1) { return n; }
    return fib(n - 1) + fib(n - 2);
}
print(fib(10));   // 55
```

支持：`int/float/string/bool` 值、`let` 变量声明、算术/比较/逻辑运算、`if/else`、`while`、
自定义函数（参数 + 返回值）、`print` 内置输出、`//` 行注释。

## 构建（Windows 原生：CMake + MinGW）

```bash
cd miniLang
cmake -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++
cmake --build build
# 产物：build\mini.exe
```

要求：已安装 CMake 与 MinGW-w64（含 `g++` 与 `mingw32-make`），并把 MinGW 的 `bin` 加入 `PATH`。

## 运行 —— 一条命令看完整条编译管线

```bash
./build/mini.exe miniLang/examples/fib.mini --tokens --ast --ir --run
```

| 选项 | 对应编译阶段 | 输出 |
|------|--------------|------|
| `--tokens` | 词法分析 | 记号流（每个 Token 的类型 + 原文 + 行号）|
| `--ast` | 语法分析 | 抽象语法树（自顶向下打印）|
| `--ir` | 中间代码生成 | 三地址中间代码（临时变量 + 标签跳转）|
| `--run` | 语义 + 解释执行 | 程序运行输出（默认）|

## 目录 / 代码与编译原理概念对应

```
miniLang/
├── token.h              词法单元/终结符（Token 类型定义）
├── lexer.h/.cpp         词法分析（字符流→记号流，有限自动机的手工实现）
├── ast.h                抽象语法树（AST 节点定义）
├── parser.h/.cpp        语法分析（递归下降，自顶向下；优先级分层表达式）
├── ir.h/.cpp            中间代码生成（AST→三地址码；含常量折叠优化示例）
├── interpreter.h/.cpp   语义+解释执行（符号表/作用域/栈帧；执行三段地址）
├── main.cpp             CLI 入口，按阶段查看
├── examples/            示例程序（basic/fib/let_if）
```

## 约定与取舍（学习角度）

- **"静态"还是"动态"？** 语言用 `let` 声明变量，值本身带类型标签，运行时按操作数类型做算术/字符串拼接/比较，属于**动态类型语义**。真要做严格静态类型检查可以再加一层符号表类型检查（见文档扩展方向）。
- **解释器执行的是中间代码**，而不是直接执行 AST —— 这正是把"中端三地址码"这一层真正跑起来的原因，本质和很多字节码虚拟机一样。
- **不做后端**：未生成汇编/机器码，聚焦前端 + 中端概念。