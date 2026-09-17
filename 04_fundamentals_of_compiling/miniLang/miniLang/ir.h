// ir.h —— 三地址中间代码（Three-Address Code, TAC）定义
//
// 对应编译原理概念：中间代码生成 / 三地址码。
// 三地址码的"三地址"指：每条指令右侧至多有一个运算符、至多两个操作数，
// 结果被写进一个独立的"地址"（临时变量 t0, t1, ...）。
//   例： a * b + c   →   t0 = a * b
//                      t1 = t0 + c   （c 作为常量可直接做操作数）
// 为什么要有中间代码？因为它与具体源语言和具体机器都无关，是一种通用枢纽：
// 一方面不同的高级语言都可翻译成它，另一方面它又便于在其上做各种优化，
// 最后再统一生成目标代码。本项目用解释器直接执行这段中间代码（类似字节码 VM）。
//
// 控制流（if/while/函数）用"标签 + 跳转"表达，和真实机器/字节码一致。

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace mini {

// 三地址指令的操作数：要么是临时变量/用户变量（运行时取环境），要么是常量。
struct Operand {
    enum class T { Temp, Var, I, F, S, B } type;
    std::string name;   // Temp / Var 时有效
    long long ival = 0; // I
    double fval = 0.0;  // F
    std::string sval;   // S
    bool bval = false;  // B

    std::string toString() const; // 用于 --ir 打印
};

// 一条三地址指令。
struct Instr {
    enum class K {
        Decl,       // var = a           （let 声明：首次建立变量）
        StoreVar,   // var = a           （赋值：变量必须已声明）
        LoadVar,    // t   = a           （把变量/常量读到临时变量）
        Unary,      // t   = op a        （op: neg / not）
        Binary,     // t   = a op b      （op: + - * / % == != < <= > >= && ||）
        Call,       // t   = call name(args...)
        Jump,       // goto L
        JumpIfFalse,// if !cond goto L   （cond 已在临时变量里：零地址）
        Ret,        // return a
        Print,      // print a
        Label,      // L:               （仅作打印/占位，执行时跳过）
    } kind;

    std::string dst;              // 结果写向的临时变量或用户变量（StoreVar/LoadVar/Unary/Binary/Call/Print）
    std::string op;               // Unary/Binary 的运算符
    Operand a, b;                 // 最多两个操作数（三地址）
    std::string callName;         // Call: 被调函数名
    std::vector<Operand> args;    // Call: 实参
    std::string label;            // Label/Jump/JumpIfFalse 的符号标签（生成期）
    int target = -1;              // 解析后：跳转目标在函数体内的指令下标

    std::string toString() const; // 用于 --ir 打印
};

// 一个函数：参数表 + 一段三地址指令。
struct IRFunc {
    std::string name;
    std::vector<std::string> params;
    std::vector<Instr> code;
};

// 中间代码程序 = 若干函数 + 顶层"main"。
// 约定：顶层语句都被收进一个名为 "main" 的函数，入口与普通函数一致。
struct IRProgram {
    std::vector<std::unique_ptr<IRFunc>> funcs;

    const IRFunc* find(const std::string& name) const;
};

// 从 AST 生成三地址中间代码。失败时抛 std::runtime_error。
std::unique_ptr<IRProgram> generateIR(const class Program& program);

// 打印中间代码（--ir 用）。
void printIR(const IRProgram& program);

} // namespace mini