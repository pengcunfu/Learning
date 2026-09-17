// ast.h —— 抽象语法树（Abstract Syntax Tree）节点定义
//
// 对应编译原理概念：抽象语法树（AST）。
// 语法分析器把记号流还原成程序的层次结构。真正的语法分析树(parse tree / concrete
// syntax tree)会保留括号、分号、每步推导的节点；而"抽象"语法树则丢掉这些对后续阶段
// 无关的细节，只保留结构。一个表达式如 (a+b)*c 会被直接表示成一棵运算子树，而不是
// 保留括号本身。
//
// 本项目后续阶段只需要 AST，因此语法分析器的输出就是这里定义的节点。

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace mini {

// ----- 表达式 -----
struct Expr {
    enum class Kind { Int, Float, String, Bool, Var, Unary, Binary, Call, Group };
    Kind kind;
    int line = 0;
    virtual ~Expr() = default;
};

struct IntLit : Expr {
    long long value;
    IntLit(long long v) { kind = Kind::Int; value = v; }
};
struct FloatLit : Expr {
    double value;
    FloatLit(double v) { kind = Kind::Float; value = v; }
};
struct StringLit : Expr {
    std::string value;
    StringLit(std::string v) { kind = Kind::String; value = std::move(v); }
};
struct BoolLit : Expr {
    bool value;
    BoolLit(bool v) { kind = Kind::Bool; value = v; }
};
struct VarExpr : Expr {
    std::string name;
    VarExpr(std::string n) { kind = Kind::Var; name = std::move(n); }
};
enum class UnaryOp { Neg, Not };
struct UnaryExpr : Expr {
    UnaryOp op;
    std::unique_ptr<Expr> operand;
    UnaryExpr(UnaryOp o, std::unique_ptr<Expr> e) { kind = Kind::Unary; op = o; operand = std::move(e); }
};
enum class BinOp { Add, Sub, Mul, Div, Mod, Eq, Neq, Lt, Le, Gt, Ge, And, Or };
struct BinaryExpr : Expr {
    BinOp op;
    std::unique_ptr<Expr> left, right;
    BinaryExpr(BinOp o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r) {
        kind = Kind::Binary; op = o; left = std::move(l); right = std::move(r);
    }
};
// 表达式里被括号括起来的部分。为简化遍历，括号节点保留，承载优先级信息。
struct GroupExpr : Expr {
    std::unique_ptr<Expr> inner;
    GroupExpr(std::unique_ptr<Expr> e) { kind = Kind::Group; inner = std::move(e); }
};
struct CallExpr : Expr {
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;
    CallExpr(std::string c) { kind = Kind::Call; callee = std::move(c); }
};

// ----- 语句 -----
struct Stmt {
    enum class Kind { Let, Assign, Return, Print, If, While, Block, Expr, FuncDecl };
    Kind kind;
    int line = 0;
    virtual ~Stmt() = default;
};

struct LetStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> init; // 可为空（只声明不初始化）
    LetStmt(std::string n) { kind = Kind::Let; name = std::move(n); }
};
struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignStmt(std::string n) { kind = Kind::Assign; name = std::move(n); }
};
struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> value;
    ReturnStmt() { kind = Kind::Return; }
};
struct PrintStmt : Stmt {
    std::unique_ptr<Expr> expr;
    PrintStmt() { kind = Kind::Print; }
};
struct IfStmt : Stmt {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch; // 可为空
    IfStmt() { kind = Kind::If; }
};
struct WhileStmt : Stmt {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> body;
    WhileStmt() { kind = Kind::While; }
};
struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> stmts;
    BlockStmt() { kind = Kind::Block; }
};
struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
    ExprStmt() { kind = Kind::Expr; }
};
struct FuncDecl : Stmt {
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<BlockStmt> body;
    FuncDecl(std::string n) { kind = Kind::FuncDecl; name = std::move(n); }
};

// ----- 根节点 -----
struct Program {
    std::vector<std::unique_ptr<Stmt>> funcs; // 函数定义
    std::vector<std::unique_ptr<Stmt>> stmts; // 顶层语句（print 等）
};

// ----- 通用遍历打印（--ast 用），递归打印节点树 -----
void printAst(const Program& program);

} // namespace mini