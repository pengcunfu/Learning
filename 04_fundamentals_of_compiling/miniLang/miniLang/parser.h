// parser.h —— 语法分析器（递归下降）声明
//
// 对应编译原理概念：语法分析（Parser / Recursive Descent / 自顶向下分析）。
// 输入一个记号流，输出一棵 AST。采用"递归下降"这种最直观的自顶向下实现：
// 每个语法非终结符对应一个函数，函数之间的调用关系直接反映了语法的结构。
//
// 表达式按"优先级分层"（precedence climbing 的级联写法）：
//   expr <- or <- and <- equality <- relational <- additive <- multiplicative <- unary <- primary
// 优先级从低到高。这样 + 的优先级低于 *，a + b * c 会被解析成 a + (b * c)。

#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "ast.h"
#include "token.h"

namespace mini {

// 语法错误，携带行号便于报告。
class ParseError : public std::runtime_error {
public:
    ParseError(int line, const std::string& msg)
        : std::runtime_error("语法错误 [行 " + std::to_string(line) + "]: " + msg) {}
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    // 入口：解析整个程序，返回 Program。
    std::unique_ptr<Program> parseProgram();

private:
    std::vector<Token> toks_;
    int idx_ = 0;

    const Token& cur() const { return toks_[idx_]; }
    const Token& peek(int n) const;        // 前看 n 个记号
    bool check(TokenType t) const;
    bool match(TokenType t);               // 若匹配则消费
    const Token& consume(TokenType t, const char* what); // 消费并校验，否则报错

    std::unique_ptr<Stmt> parseStmt();
    std::unique_ptr<Stmt> parseBlock();
    std::unique_ptr<Stmt> parseFuncDecl();
    std::unique_ptr<Stmt> parseLet();
    std::unique_ptr<Stmt> parseAssign();
    std::unique_ptr<Stmt> parseReturn();
    std::unique_ptr<Stmt> parsePrint();
    std::unique_ptr<Stmt> parseIf();
    std::unique_ptr<Stmt> parseWhile();

    // 表达式优先级链
    std::unique_ptr<Expr> parseExpr();
    std::unique_ptr<Expr> parseOr();
    std::unique_ptr<Expr> parseAnd();
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseRelational();
    std::unique_ptr<Expr> parseAdditive();
    std::unique_ptr<Expr> parseMultiplicative();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parsePrimary();

    void expectSemi();
};

// AST 打印（在 parser.cpp 中实现）
void doPrintStmt(const Stmt& s, int depth);

} // namespace mini