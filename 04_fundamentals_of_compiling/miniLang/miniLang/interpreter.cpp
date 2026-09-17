// interpreter.cpp —— 三地址 IR 解释器实现
//
// 主循环：取出栈顶帧的当前指令 → 按其类型执行 → 前进指令指针。
// 条件/无条件跳转改变指令指针；函数调用压入新帧，return 弹出帧并把结果交给调用方。

#include "interpreter.h"

#include <cstdio>
#include <map>
#include <stdexcept>
#include <vector>

namespace mini {

double Interpreter::Value::asDouble() const {
    return (type == T::I) ? static_cast<double>(i)
         : (type == T::F) ? f
         : (type == T::B) ? (b ? 1.0 : 0.0) : 0.0;
}
bool Interpreter::Value::truthy() const {
    switch (type) {
        case T::I: return i != 0;
        case T::F: return f != 0.0;
        case T::S: return !s.empty();
        case T::B: return b;
    }
    return false;
}

Interpreter::Interpreter(const IRProgram* program) : prog_(program) {}

// 把操作数求值为运行时值：常量直接返回，变量/临时变量查当前帧环境。
Interpreter::Value Interpreter::evalOperand(const Operand& o, Frame& f) const {
    Value v;
    switch (o.type) {
        case Operand::T::I: v.type = Value::T::I; v.i = o.ival; break;
        case Operand::T::F: v.type = Value::T::F; v.f = o.fval; break;
        case Operand::T::S: v.type = Value::T::S; v.s = o.sval; break;
        case Operand::T::B: v.type = Value::T::B; v.b = o.bval; break;
        case Operand::T::Temp:
        case Operand::T::Var: {
            auto it = f.env.find(o.name);
            if (it == f.env.end())
                throw std::runtime_error("运行时错误：变量未定义 '" + o.name + "'（可能未声明或不在作用域内）");
            return it->second;
        }
    }
    return v;
}

Interpreter::Value Interpreter::doUnary(const std::string& op, const Value& a) const {
    Value r;
    if (op == "neg") {
        r = a;
        if (a.type == Value::T::F) r.f = -a.f;
        else if (a.type == Value::T::I) r.i = -a.i;
        else throw std::runtime_error("运行时错误：无法对非数值取负");
        return r;
    } else { // not
        r.type = Value::T::B; r.b = !a.truthy(); return r;
    }
}

Interpreter::Value Interpreter::doBinary(const std::string& op, const Value& l, const Value& r) const {
    // 逻辑运算：按"真值"处理
    if (op == "&&") { Value v; v.type = Value::T::B; v.b = l.truthy() && r.truthy(); return v; }
    if (op == "||") { Value v; v.type = Value::T::B; v.b = l.truthy() || r.truthy(); return v; }

    // '+'：数值求和或字符串拼接
    if (op == "+") {
        if (l.type == Value::T::S && r.type == Value::T::S) {
            Value v; v.type = Value::T::S; v.s = l.s + r.s; return v;
        }
        if (l.type == Value::T::F || r.type == Value::T::F) {
            Value v; v.type = Value::T::F; v.f = l.asDouble() + r.asDouble(); return v;
        }
        Value v; v.type = Value::T::I; v.i = l.i + r.i; return v;
    }

    // 数值算术
    bool isFloat = (l.type == Value::T::F || r.type == Value::T::F);
    if (isFloat) {
        double a = l.asDouble(), b = r.asDouble();
        Value v; v.type = Value::T::F;
        if (op == "-") v.f = a - b;
        else if (op == "*") v.f = a * b;
        else if (op == "/") { if (b == 0) throw std::runtime_error("运行时错误：除数不能为 0"); v.f = a / b; }
        else throw std::runtime_error("运行时错误：不支持的运算符 " + op);
        return v;
    }
    // 整数算术
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
        Value v; v.type = Value::T::I;
        if (op == "+") v.i = l.i + r.i;
        else if (op == "-") v.i = l.i - r.i;
        else if (op == "*") v.i = l.i * r.i;
        else if (op == "/") { if (r.i == 0) throw std::runtime_error("运行时错误：除数不能为 0"); v.i = l.i / r.i; }
        else { if (r.i == 0) throw std::runtime_error("运行时错误：除数不能为 0"); v.i = l.i % r.i; }
        return v;
    }

    // 比较运算：数值比大小、字符串/布尔按值相等判等
    bool bothStr = (l.type == Value::T::S && r.type == Value::T::S);
    bool bothNum = (l.type == Value::T::I || l.type == Value::T::F) &&
                   (r.type == Value::T::I || r.type == Value::T::F);
    bool ok;
    if (op == "==") {
        ok = (bothStr) ? l.s == r.s
           : (bothNum) ? l.asDouble() == r.asDouble()
           : (l.type == r.type && l.b == r.b);
    } else if (op == "!=") {
        ok = (bothStr) ? l.s != r.s
           : (bothNum) ? l.asDouble() != r.asDouble()
           : !(l.type == r.type && l.b == r.b);
    } else { // < <= > >= 仅数值
        double a = l.asDouble(), b = r.asDouble();
        if (op == "<") ok = a < b; else if (op == "<=") ok = a <= b;
        else if (op == ">") ok = a > b; else ok = a >= b;
    }
    Value v; v.type = Value::T::B; v.b = ok; return v;
}

void Interpreter::printValue(const Value& v) const {
    switch (v.type) {
        case Value::T::I: std::printf("%lld\n", v.i); break;
        case Value::T::F: std::printf("%g\n", v.f); break;
        case Value::T::S: std::printf("%s\n", v.s.c_str()); break;
        case Value::T::B: std::printf("%s\n", v.b ? "true" : "false"); break;
    }
}

// 调用函数：求值实参 → 建立新帧并压栈；返回结果由 Ret 指令在弹出帧时写回调用方。
void Interpreter::call(const std::string& name, const std::vector<Operand>& args, Frame& caller) {
    const IRFunc* fn = prog_->find(name);
    if (!fn) throw std::runtime_error("运行时错误：调用未定义的函数 '" + name + "'");
    if (args.size() != fn->params.size())
        throw std::runtime_error("运行时错误：函数 '" + name + "' 实参个数应为 " +
            std::to_string(fn->params.size()) + "，实际 " + std::to_string(args.size()));

    Frame callee;
    callee.fn = fn;
    callee.ip = 0;
    for (size_t i = 0; i < fn->params.size(); i++) {
        callee.env[fn->params[i]] = evalOperand(args[i], caller);
    }
    callee.retDst = caller_env_dst_; // 由执行循环设置在调用方写的目标
    callee.backIp = caller_back_ip_;
    stack_.push_back(std::move(callee));
}

int Interpreter::run() {
    const IRFunc* main = prog_->find("main");
    if (!main) throw std::runtime_error("没有可运行的入口（顶层语句为空）");

    Frame root;
    root.fn = main;
    root.ip = 0;
    stack_.push_back(std::move(root));

    while (!stack_.empty()) {
        Frame& f = stack_.back();

        // 函数体执行到末尾而未 return，视为返回 0（隐式 return 0）
        if (f.ip >= static_cast<int>(f.fn->code.size())) {
            std::string retTo = f.retDst;
            int back = f.backIp;
            Value val{}; val.type = Value::T::I; val.i = 0;
            stack_.pop_back();
            if (!stack_.empty()) {
                stack_.back().env[retTo] = val;
                stack_.back().ip = back;
            }
            continue;
        }

        const Instr& in = f.fn->code[f.ip];
        switch (in.kind) {
            case Instr::K::Label: f.ip++; break;

            case Instr::K::LoadVar: {
                f.env[in.dst] = evalOperand(in.a, f);
                f.ip++;
                break;
            }
            case Instr::K::Decl: {
                // let 声明：建立（或覆盖）变量
                f.env[in.dst] = evalOperand(in.a, f);
                f.ip++;
                break;
            }
            case Instr::K::StoreVar: {
                // 赋值：要求变量已声明，否则报错（区分 let 声明与赋值）
                if (f.env.find(in.dst) == f.env.end())
                    throw std::runtime_error("运行时错误：赋值给未声明的变量 '" + in.dst + "'");
                f.env[in.dst] = evalOperand(in.a, f);
                f.ip++;
                break;
            }
            case Instr::K::Unary: {
                f.env[in.dst] = doUnary(in.op, evalOperand(in.a, f));
                f.ip++;
                break;
            }
            case Instr::K::Binary: {
                f.env[in.dst] = doBinary(in.op, evalOperand(in.a, f), evalOperand(in.b, f));
                f.ip++;
                break;
            }
            case Instr::K::Call: {
                // 记录调用方的写入目标与返回位置
                caller_env_dst_ = in.dst;
                caller_back_ip_ = f.ip + 1;
                call(in.callName, in.args, f);
                // call 已压入新帧，这里不推进 ip；下一轮循环转向新帧
                break;
            }
            case Instr::K::Print: {
                printValue(evalOperand(in.a, f));
                f.ip++;
                break;
            }
            case Instr::K::Ret: {
                Value val = evalOperand(in.a, f);
                std::string retTo = f.retDst;
                int back = f.backIp;
                stack_.pop_back();
                if (!stack_.empty()) {
                    stack_.back().env[retTo] = val;
                    stack_.back().ip = back;
                }
                break;
            }
            case Instr::K::Jump: {
                f.ip = in.target;
                break;
            }
            case Instr::K::JumpIfFalse: {
                if (!evalOperand(in.a, f).truthy()) f.ip = in.target;
                else f.ip++;
                break;
            }
        }
    }
    return resultCode_;
}

} // namespace mini