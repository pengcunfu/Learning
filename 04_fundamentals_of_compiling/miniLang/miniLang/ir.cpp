// ir.cpp —— AST → 三地址中间代码生成器
//
// 生成策略（经典"语法制导翻译"，syntax-directed translation）：
//   每个表达式生成代码后返回一个"结果句柄"（Operand，通常是临时变量名）。
//   上层表达式把下层结果作为操作数，拼成一条三地址指令，再新建一个临时变量承接结果。
// 这样 AST 的后序遍历 == 中间代码的生成顺序，直观且正确。
//
// 常量折叠（constant folding）：若二元运算左右都是字面量，直接算完放进结果操作数，
// 不生成指令，体现中端一个最基础也是最有代表性的优化。

#include "ir.h"
#include "ast.h"

#include <cstdio>
#include <map>
#include <stdexcept>
#include <sstream>

namespace mini {

// ---------------- Operand 与 Instr 打印 ----------------
std::string Operand::toString() const {
    switch (type) {
        case T::Temp: return name;
        case T::Var:  return name;
        case T::I:    return std::to_string(ival);
        case T::F:    { char buf[32]; std::snprintf(buf, sizeof buf, "%g", fval); return buf; }
        case T::S:    return "\"" + sval + "\"";
        case T::B:    return bval ? "true" : "false";
    }
    return "?";
}

std::string Instr::toString() const {
    switch (kind) {
        case K::Decl:       return "decl " + dst + " = " + a.toString();
        case K::StoreVar:   return dst + " = " + a.toString();
        case K::LoadVar:    return dst + " = " + a.toString();
        case K::Unary:      return dst + " = " + op + " " + a.toString();
        case K::Binary:     return dst + " = " + a.toString() + " " + op + " " + b.toString();
        case K::Call: {
            std::string s = dst + " = call " + callName + "(";
            for (size_t i = 0; i < args.size(); i++) { if (i) s += ", "; s += args[i].toString(); }
            return s + ")";
        }
        case K::Jump:       return "goto L" + std::to_string(target);
        case K::JumpIfFalse:return "if !" + a.toString() + " goto L" + std::to_string(target);
        case K::Ret:        return "return " + a.toString();
        case K::Print:      return "print " + a.toString();
        case K::Label:      return "L" + std::to_string(target) + ":";
    }
    return "?";
}

// ---------------- IR 生成器 ----------------
namespace {

class IRGen {
public:
    IRProgram& out;
    IRFunc* cur = nullptr; // 当前正在生成的函数

    int tempCount = 0;
    int labelCount = 0;
    using LabelMap = std::map<std::string, int>;
    LabelMap labelIndex; // 符号标签 → 指令下标（Label 指令被 push 时记录，整型稳定可靠）

    explicit IRGen(IRProgram& program) : out(program) {}

    void push(Instr in) {
        cur->code.push_back(std::move(in));
        // 仅当这是 Label 指令时登记其在函数体内的下标。
        // 注意：不要在 push 里缓存指向指令的指针——vector 扩容会使指针全部失效。
        if (cur->code.back().kind == Instr::K::Label) {
            labelIndex[cur->code.back().label] = static_cast<int>(cur->code.size()) - 1;
        }
    }

    // 把所有 jump 的符号标签解析为绝对下标。
    // 一次性遍历当前函数体即可，jump 指令就地改写 target，无需指针缓存。
    void resolveJumps() {
        for (auto& in : cur->code) {
            if (in.kind == Instr::K::Label) {
                auto it = labelIndex.find(in.label);
                in.target = (it != labelIndex.end()) ? it->second : 0;
            } else if (in.kind == Instr::K::Jump || in.kind == Instr::K::JumpIfFalse) {
                auto it = labelIndex.find(in.label);
                in.target = (it != labelIndex.end()) ? it->second
                           : (cur->code.empty() ? 0 : static_cast<int>(cur->code.size()) - 1);
            }
        }
        labelIndex.clear();
    }

    std::string newTemp() { return "t" + std::to_string(tempCount++); }
    std::string newLabel() { return "$L" + std::to_string(labelCount++); }

    static Operand litInt(long long v) { Operand o; o.type = Operand::T::I; o.ival = v; return o; }
    static Operand litFloat(double v) { Operand o; o.type = Operand::T::F; o.fval = v; return o; }
    static Operand litStr(std::string s) { Operand o; o.type = Operand::T::S; o.sval = std::move(s); return o; }
    static Operand litBool(bool b) { Operand o; o.type = Operand::T::B; o.bval = b; return o; }
    static Operand varOp(std::string n) { Operand o; o.type = Operand::T::Var; o.name = std::move(n); return o; }
    static Operand tempOp(std::string n) { Operand o; o.type = Operand::T::Temp; o.name = std::move(n); return o; }

    // ---- 语句生成 ----
    void genStmt(const Stmt& s) {
        switch (s.kind) {
            case Stmt::Kind::Let: {
                const auto& l = static_cast<const LetStmt&>(s);
                if (l.init) {
                    Operand v = genExpr(*l.init);
                    Instr in; in.kind = Instr::K::Decl; in.dst = l.name; in.a = v; push(std::move(in));
                } else {
                    Instr in; in.kind = Instr::K::Decl; in.dst = l.name; in.a = litInt(0); push(std::move(in));
                }
                break;
            }
            case Stmt::Kind::Assign: {
                const auto& a = static_cast<const AssignStmt&>(s);
                Instr in; in.kind = Instr::K::StoreVar; in.dst = a.name; in.a = genExpr(*a.value); push(std::move(in));
                break;
            }
            case Stmt::Kind::Return: {
                const auto& r = static_cast<const ReturnStmt&>(s);
                Instr in; in.kind = Instr::K::Ret;
                in.a = r.value ? genExpr(*r.value) : litInt(0);
                push(std::move(in));
                break;
            }
            case Stmt::Kind::Print: {
                Instr in; in.kind = Instr::K::Print; in.a = genExpr(*static_cast<const PrintStmt&>(s).expr); push(std::move(in));
                break;
            }
            case Stmt::Kind::Expr: {
                genExpr(*static_cast<const ExprStmt&>(s).expr); // 结果被丢弃，但副作用（赋值/调用）仍在
                break;
            }
            case Stmt::Kind::If: {
                const auto& i = static_cast<const IfStmt&>(s);
                Operand cond = genExpr(*i.cond);
                std::string elseL = newLabel(), endL = newLabel();
                Instr jf; jf.kind = Instr::K::JumpIfFalse; jf.a = cond; jf.label = elseL; push(std::move(jf));
                genStmt(*i.thenBranch);
                Instr jmp; jmp.kind = Instr::K::Jump; jmp.label = endL; push(std::move(jmp));
                Instr lab1; lab1.kind = Instr::K::Label; lab1.label = elseL; push(std::move(lab1));
                if (i.elseBranch) genStmt(*i.elseBranch);
                Instr lab2; lab2.kind = Instr::K::Label; lab2.label = endL; push(std::move(lab2));
                break;
            }
            case Stmt::Kind::While: {
                const auto& w = static_cast<const WhileStmt&>(s);
                std::string startL = newLabel(), endL = newLabel();
                Instr lab1; lab1.kind = Instr::K::Label; lab1.label = startL; push(std::move(lab1));
                Operand cond = genExpr(*w.cond);
                Instr jf; jf.kind = Instr::K::JumpIfFalse; jf.a = cond; jf.label = endL; push(std::move(jf));
                genStmt(*w.body);
                Instr jmp; jmp.kind = Instr::K::Jump; jmp.label = startL; push(std::move(jmp));
                Instr lab2; lab2.kind = Instr::K::Label; lab2.label = endL; push(std::move(lab2));
                break;
            }
            case Stmt::Kind::Block: {
                for (const auto& st : static_cast<const BlockStmt&>(s).stmts) genStmt(*st);
                break;
            }
            case Stmt::Kind::FuncDecl: {
                // 已在 IRProgram 层面单独处理，这里不会走到
                break;
            }
        }
    }

    // ---- 表达式生成：返回操作数 ----
    Operand genExpr(const Expr& e) {
        switch (e.kind) {
            case Expr::Kind::Int: {
                const auto& n = static_cast<const IntLit&>(e);
                std::string t = newTemp(); Instr in; in.kind = Instr::K::LoadVar; in.dst = t; in.a = litInt(n.value); push(std::move(in)); return tempOp(t);
            }
            case Expr::Kind::Float: {
                const auto& n = static_cast<const FloatLit&>(e);
                std::string t = newTemp(); Instr in; in.kind = Instr::K::LoadVar; in.dst = t; in.a = litFloat(n.value); push(std::move(in)); return tempOp(t);
            }
            case Expr::Kind::String: {
                const auto& n = static_cast<const StringLit&>(e);
                std::string t = newTemp(); Instr in; in.kind = Instr::K::LoadVar; in.dst = t; in.a = litStr(n.value); push(std::move(in)); return tempOp(t);
            }
            case Expr::Kind::Bool: {
                const auto& n = static_cast<const BoolLit&>(e);
                std::string t = newTemp(); Instr in; in.kind = Instr::K::LoadVar; in.dst = t; in.a = litBool(n.value); push(std::move(in)); return tempOp(t);
            }
            case Expr::Kind::Var: {
                const auto& n = static_cast<const VarExpr&>(e);
                std::string t = newTemp(); Instr in; in.kind = Instr::K::LoadVar; in.dst = t; in.a = varOp(n.name); push(std::move(in)); return tempOp(t);
            }
            case Expr::Kind::Group: {
                return genExpr(*static_cast<const GroupExpr&>(e).inner);
            }
            case Expr::Kind::Unary: {
                const auto& u = static_cast<const UnaryExpr&>(e);
                Operand a = genExpr(*u.operand);
                std::string t = newTemp(); Instr in; in.kind = Instr::K::Unary;
                in.dst = t; in.op = (u.op == UnaryOp::Neg) ? "neg" : "not"; in.a = a;
                push(std::move(in)); return tempOp(t);
            }
            case Expr::Kind::Binary: {
                const auto& b = static_cast<const BinaryExpr&>(e);
                Operand l = genExpr(*b.left);
                Operand r = genExpr(*b.right);
                // 常量折叠：左右皆是整数常量时直接算，不生成指令（最基础的优化）
                if (l.type == Operand::T::I && r.type == Operand::T::I && intFoldOk(b.op)) {
                    return litInt(foldInt(b.op, l.ival, r.ival));
                }
                std::string t = newTemp(); Instr in; in.kind = Instr::K::Binary;
                in.dst = t; in.op = opStr(b.op); in.a = l; in.b = r;
                push(std::move(in)); return tempOp(t);
            }
            case Expr::Kind::Call: {
                const auto& c = static_cast<const CallExpr&>(e);
                std::string t = newTemp(); Instr in; in.kind = Instr::K::Call;
                in.dst = t; in.callName = c.callee;
                for (const auto& arg : c.args) in.args.push_back(genExpr(*arg));
                push(std::move(in)); return tempOp(t);
            }
        }
        throw std::runtime_error("ir: 未知表达式节点");
    }

    static bool intFoldOk(BinOp op) {
        return op == BinOp::Add || op == BinOp::Sub || op == BinOp::Mul ||
               op == BinOp::Div || op == BinOp::Mod ||
               op == BinOp::Eq || op == BinOp::Neq;
    }
    static long long foldInt(BinOp op, long long l, long long r) {
        switch (op) {
            case BinOp::Add: return l + r;
            case BinOp::Sub: return l - r;
            case BinOp::Mul: return l * r;
            case BinOp::Div: return r == 0 ? 0 : l / r;
            case BinOp::Mod: return r == 0 ? 0 : l % r;
            case BinOp::Eq:  return l == r;
            case BinOp::Neq: return l != r;
            default: return 0;
        }
    }
    static const char* opStr(BinOp op) {
        switch (op) { case BinOp::Add: return "+"; case BinOp::Sub: return "-";
            case BinOp::Mul: return "*"; case BinOp::Div: return "/"; case BinOp::Mod: return "%";
            case BinOp::Eq: return "=="; case BinOp::Neq: return "!="; case BinOp::Lt: return "<";
            case BinOp::Le: return "<="; case BinOp::Gt: return ">"; case BinOp::Ge: return ">=";
            case BinOp::And: return "&&"; case BinOp::Or: return "||"; }
        return "?";
    }
};

} // namespace

std::unique_ptr<IRProgram> generateIR(const Program& program) {
    auto prog = std::make_unique<IRProgram>();
    IRGen gen(*prog);

    // 顶层"main"函数
    auto main = std::make_unique<IRFunc>();
    main->name = "main";
    prog->funcs.push_back(std::move(main));
    gen.cur = prog->funcs.back().get();
    for (const auto& s : program.stmts) gen.genStmt(*s);
    gen.resolveJumps();

    // 普通函数
    for (const auto& f : program.funcs) {
        const auto& fd = static_cast<const FuncDecl&>(*f);
        auto fn = std::make_unique<IRFunc>();
        fn->name = fd.name;
        fn->params = fd.params;
        prog->funcs.push_back(std::move(fn));
        gen.cur = prog->funcs.back().get();
        gen.genStmt(*fd.body);
        // 隐式结尾 return 0（省略 return 的函数视为返回 0）
        Instr ret; ret.kind = Instr::K::Ret; ret.a = Operand{}; ret.a.type = Operand::T::I; ret.a.ival = 0;
        gen.cur->code.push_back(std::move(ret));
        gen.resolveJumps();
    }
    return prog;
}

const IRFunc* IRProgram::find(const std::string& name) const {
    for (const auto& f : funcs) if (f->name == name) return f.get();
    return nullptr;
}

void printIR(const IRProgram& program) {
    std::printf("==== 三地址中间代码 (IR) ====\n");
    for (const auto& f : program.funcs) {
        std::printf("func %s(", f->name.c_str());
        for (size_t i = 0; i < f->params.size(); i++) { if (i) std::printf(", "); std::printf("%s", f->params[i].c_str()); }
        std::printf("):\n");
        int n = 0;
        for (const auto& in : f->code) {
            std::printf("  %3d %s\n", n++, in.toString().c_str());
        }
        std::printf("\n");
    }
}

} // namespace mini