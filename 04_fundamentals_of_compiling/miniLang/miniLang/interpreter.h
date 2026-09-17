// interpreter.h —— 三地址 IR 解释器（小虚拟机）声明
//
// 对应编译原理概念：语义分析 + 解释执行（虚拟机）。
//  - 符号表 / 环境：每个函数运行时一个"环境"，保存变量名到值的映射；
//    函数调用用"栈帧"管理，帧里既存局部变量，也存返回地址——这就是运行时作用域。
//  - 类型：Value 是"标签联合"（int/float/string/bool），运算符按操作数运行时类型
//    决定算术/字符串拼接/比较，体现动态类型的语义。
//  - 它执行的是我们前面生成的中间代码，而不是直接执行 AST——
//    这正是"把中间代码这一层真正跑起来"的做法。

#pragma once

#include <map>
#include <string>
#include <vector>

#include "ir.h"

namespace mini {

class Interpreter {
public:
    explicit Interpreter(const IRProgram* program);

    // 运行整个程序（从名称为 "main" 的顶层函数开始）。
    int run();

private:
    const IRProgram* prog_;

    // 运行时值：标签联合。
    struct Value {
        enum T { I, F, S, B } type = I;
        long long i = 0;
        double f = 0.0;
        std::string s;
        bool b = false;

        double asDouble() const;
        bool   truthy() const;
    };

    // 调用栈帧：函数 + 指令指针 + 本地环境 + 返回信息。
    struct Frame {
        const IRFunc* fn;
        int ip = 0;
        std::map<std::string, Value> env; // 局部符号表（含参数、临时变量）
        std::string retDst;               // 返回后要写入的调用方临时变量名
        int backIp = 0;                   // 返回后要恢复的调用方指令位置
    };

    std::vector<Frame> stack_;
    std::string caller_env_dst_; // call 指令：调用方临时变量写入目标
    int caller_back_ip_ = 0;     // call 指令：调用方返回位置
    int resultCode_ = 0;         // 程序退出码（当前恒为 0）

    Value evalOperand(const Operand& o, Frame& f) const;
    Value doBinary(const std::string& op, const Value& l, const Value& r) const;
    Value doUnary(const std::string& op, const Value& a) const;
    void printValue(const Value& v) const;
    void call(const std::string& name, const std::vector<Operand>& args, Frame& caller);
};

} // namespace mini