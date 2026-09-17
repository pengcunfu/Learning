// parser.cpp —— 递归下降语法分析器实现 + AST 打印
//
// 说明：
//  - 每个 parseXxx 函数对应文法里的一个非终结符，函数递归互相调用即完成推导。
//  - 表达式采用优先级级联，天然解决" a + b * c 应解析成 a + (b * c) "问题。
//  - 没有用手工消除左递归，因为级联写法本身就是右结合展开，不会陷入左递归死循环。

#include "parser.h"

#include <cstdio>
#include <sstream>

namespace mini {

const Token& Parser::peek(int n) const {
    int i = idx_ + n;
    return toks_[i >= static_cast<int>(toks_.size()) ? static_cast<int>(toks_.size()) - 1 : i];
}
bool Parser::check(TokenType t) const { return cur().is(t); }
bool Parser::match(TokenType t) {
    if (check(t)) { idx_++; return true; }
    return false;
}
const Token& Parser::consume(TokenType t, const char* what) {
    if (!cur().is(t)) throw ParseError(cur().line, std::string("期望 ") + what +
        "，但遇到 '" + cur().lexeme + "'");
    const Token& ret = cur();
    idx_++;
    return ret;
}

Parser::Parser(std::vector<Token> tokens) : toks_(std::move(tokens)) {}

std::unique_ptr<Program> Parser::parseProgram() {
    auto prog = std::make_unique<Program>();
    while (!cur().is(TokenType::END)) {
        if (check(TokenType::FUNC)) prog->funcs.push_back(parseFuncDecl());
        else if (check(TokenType::LET)) prog->stmts.push_back(parseLet());
        else prog->stmts.push_back(parseStmt());
    }
    return prog;
}

std::unique_ptr<Stmt> Parser::parseFuncDecl() {
    consume(TokenType::FUNC, "func");
    Token name = consume(TokenType::IDENT, "函数名");
    consume(TokenType::LPAREN, "'('");
    auto fn = std::make_unique<FuncDecl>(name.lexeme);
    fn->line = name.line;
    if (!check(TokenType::RPAREN)) {
        do { fn->params.push_back(consume(TokenType::IDENT, "参数名").lexeme); }
        while (match(TokenType::COMMA));
    }
    consume(TokenType::RPAREN, "')'");
    auto body = dynamic_cast<BlockStmt*>(parseBlock().release());
    fn->body = std::unique_ptr<BlockStmt>(body);
    return fn;
}

std::unique_ptr<Stmt> Parser::parseBlock() {
    consume(TokenType::LBRACE, "'{'");
    auto block = std::make_unique<BlockStmt>();
    block->line = cur().line;
    while (!check(TokenType::RBRACE) && !check(TokenType::END)) {
        block->stmts.push_back(parseStmt());
    }
    consume(TokenType::RBRACE, "'}'");
    return block;
}

std::unique_ptr<Stmt> Parser::parseLet() {
    consume(TokenType::LET, "let");
    Token name = consume(TokenType::IDENT, "变量名");
    auto let = std::make_unique<LetStmt>(name.lexeme);
    let->line = name.line;
    if (match(TokenType::ASSIGN)) let->init = parseExpr();
    expectSemi();
    return let;
}

std::unique_ptr<Stmt> Parser::parseAssign() {
    Token name = consume(TokenType::IDENT, "变量名");
    consume(TokenType::ASSIGN, "'='");
    auto assign = std::make_unique<AssignStmt>(name.lexeme);
    assign->line = name.line;
    assign->value = parseExpr();
    expectSemi();
    return assign;
}

std::unique_ptr<Stmt> Parser::parseReturn() {
    consume(TokenType::RETURN, "return");
    auto ret = std::make_unique<ReturnStmt>();
    ret->line = cur().line;
    if (!check(TokenType::SEMI)) ret->value = parseExpr();
    expectSemi();
    return ret;
}

std::unique_ptr<Stmt> Parser::parsePrint() {
    consume(TokenType::PRINT, "print");
    consume(TokenType::LPAREN, "'('");
    auto p = std::make_unique<PrintStmt>();
    p->line = cur().line;
    p->expr = parseExpr();
    consume(TokenType::RPAREN, "')'");
    expectSemi();
    return p;
}

std::unique_ptr<Stmt> Parser::parseIf() {
    consume(TokenType::IF, "if");
    consume(TokenType::LPAREN, "'('");
    auto ifs = std::make_unique<IfStmt>();
    ifs->line = cur().line;
    ifs->cond = parseExpr();
    consume(TokenType::RPAREN, "')'");
    ifs->thenBranch = parseStmt();
    if (match(TokenType::ELSE)) ifs->elseBranch = parseStmt();
    return ifs;
}

std::unique_ptr<Stmt> Parser::parseWhile() {
    consume(TokenType::WHILE, "while");
    consume(TokenType::LPAREN, "'('");
    auto wh = std::make_unique<WhileStmt>();
    wh->line = cur().line;
    wh->cond = parseExpr();
    consume(TokenType::RPAREN, "')'");
    wh->body = parseStmt();
    return wh;
}

std::unique_ptr<Stmt> Parser::parseStmt() {
    if (cur().is(TokenType::IF)) return parseIf();
    if (cur().is(TokenType::WHILE)) return parseWhile();
    if (cur().is(TokenType::PRINT)) return parsePrint();
    if (cur().is(TokenType::RETURN)) return parseReturn();
    if (cur().is(TokenType::LBRACE)) return parseBlock();
    if (cur().is(TokenType::LET)) return parseLet();
    // 表达式语句（含赋值）：x = 1; 或 直接是调用  fib(2);
    if (cur().is(TokenType::IDENT) && peek(1).is(TokenType::ASSIGN)) {
        return parseAssign();
    }
    auto es = std::make_unique<ExprStmt>();
    es->line = cur().line;
    es->expr = parseExpr();
    expectSemi();
    return es;
}

// ----- 表达式优先级链（低 → 高） -----
std::unique_ptr<Expr> Parser::parseExpr() { return parseOr(); }

std::unique_ptr<Expr> Parser::parseOr() {
    auto left = parseAnd();
    while (match(TokenType::OR)) {
        auto right = parseAnd();
        left = std::make_unique<BinaryExpr>(BinOp::Or, std::move(left), std::move(right));
    }
    return left;
}
std::unique_ptr<Expr> Parser::parseAnd() {
    auto left = parseEquality();
    while (match(TokenType::AND)) {
        auto right = parseEquality();
        left = std::make_unique<BinaryExpr>(BinOp::And, std::move(left), std::move(right));
    }
    return left;
}
std::unique_ptr<Expr> Parser::parseEquality() {
    auto left = parseRelational();
    while (true) {
        if (match(TokenType::EQ)) left = std::make_unique<BinaryExpr>(BinOp::Eq, std::move(left), parseRelational());
        else if (match(TokenType::NEQ)) left = std::make_unique<BinaryExpr>(BinOp::Neq, std::move(left), parseRelational());
        else break;
    }
    return left;
}
std::unique_ptr<Expr> Parser::parseRelational() {
    auto left = parseAdditive();
    while (true) {
        Token t = cur();
        BinOp op;
        if (match(TokenType::LT)) op = BinOp::Lt;
        else if (match(TokenType::LE)) op = BinOp::Le;
        else if (match(TokenType::GT)) op = BinOp::Gt;
        else if (match(TokenType::GE)) op = BinOp::Ge;
        else break;
        left = std::make_unique<BinaryExpr>(op, std::move(left), parseAdditive());
    }
    return left;
}
std::unique_ptr<Expr> Parser::parseAdditive() {
    auto left = parseMultiplicative();
    while (true) {
        Token t = cur();
        BinOp op;
        if (match(TokenType::PLUS)) op = BinOp::Add;
        else if (match(TokenType::MINUS)) op = BinOp::Sub;
        else break;
        left = std::make_unique<BinaryExpr>(op, std::move(left), parseMultiplicative());
    }
    return left;
}
std::unique_ptr<Expr> Parser::parseMultiplicative() {
    auto left = parseUnary();
    while (true) {
        BinOp op;
        if (match(TokenType::STAR)) op = BinOp::Mul;
        else if (match(TokenType::SLASH)) op = BinOp::Div;
        else if (match(TokenType::PERCENT)) op = BinOp::Mod;
        else break;
        left = std::make_unique<BinaryExpr>(op, std::move(left), parseUnary());
    }
    return left;
}
std::unique_ptr<Expr> Parser::parseUnary() {
    if (match(TokenType::MINUS)) return std::make_unique<UnaryExpr>(UnaryOp::Neg, parseUnary());
    if (match(TokenType::NOT)) return std::make_unique<UnaryExpr>(UnaryOp::Not, parseUnary());
    return parsePrimary();
}
std::unique_ptr<Expr> Parser::parsePrimary() {
    const Token& t = cur();
    switch (t.type) {
        case TokenType::INT_LITERAL: idx_++; return std::make_unique<IntLit>(t.int_value);
        case TokenType::FLOAT_LITERAL: idx_++; return std::make_unique<FloatLit>(t.float_value);
        case TokenType::STRING_LITERAL: idx_++; return std::make_unique<StringLit>(t.string_value);
        case TokenType::TRUE: idx_++; return std::make_unique<BoolLit>(true);
        case TokenType::FALSE: idx_++; return std::make_unique<BoolLit>(false);
        case TokenType::IDENT: {
            idx_++;
            if (check(TokenType::LPAREN)) { // 函数调用
                idx_++; // '('
                auto call = std::make_unique<CallExpr>(t.lexeme);
                call->line = t.line;
                if (!check(TokenType::RPAREN)) {
                    do { call->args.push_back(parseExpr()); }
                    while (match(TokenType::COMMA));
                }
                consume(TokenType::RPAREN, "')'");
                return call;
            }
            auto v = std::make_unique<VarExpr>(t.lexeme);
            v->line = t.line;
            return v;
        }
        case TokenType::LPAREN: {
            idx_++; // '('
            auto g = std::make_unique<GroupExpr>(parseExpr());
            g->line = t.line;
            consume(TokenType::RPAREN, "')'");
            return g;
        }
        default:
            throw ParseError(t.line, "意外的记号 '" + t.lexeme + "'");
    }
}

void Parser::expectSemi() {
    if (!match(TokenType::SEMI)) throw ParseError(cur().line, "期望 ';'");
}

// ====================== AST 打印 ======================
static void indent(int d) { for (int i = 0; i < d; i++) std::printf("  "); }

static const char* binOpName(BinOp op) {
    switch (op) {
        case BinOp::Add: return "+"; case BinOp::Sub: return "-";
        case BinOp::Mul: return "*"; case BinOp::Div: return "/";
        case BinOp::Mod: return "%"; case BinOp::Eq: return "==";
        case BinOp::Neq: return "!="; case BinOp::Lt: return "<";
        case BinOp::Le: return "<="; case BinOp::Gt: return ">";
        case BinOp::Ge: return ">="; case BinOp::And: return "&&";
        case BinOp::Or: return "||";
    }
    return "?";
}
static void doPrintExpr(const Expr& e, int d) {
    switch (e.kind) {
        case Expr::Kind::Int: indent(d); std::printf("%lld\n", static_cast<const IntLit&>(e).value); break;
        case Expr::Kind::Float: indent(d); std::printf("%g\n", static_cast<const FloatLit&>(e).value); break;
        case Expr::Kind::String: indent(d); std::printf("\"%s\"\n", static_cast<const StringLit&>(e).value.c_str()); break;
        case Expr::Kind::Bool: indent(d); std::printf("%s\n", static_cast<const BoolLit&>(e).value ? "true" : "false"); break;
        case Expr::Kind::Var: indent(d); std::printf("var %s\n", static_cast<const VarExpr&>(e).name.c_str()); break;
        case Expr::Kind::Unary: {
            const auto& u = static_cast<const UnaryExpr&>(e);
            indent(d); std::printf("%s\n", u.op == UnaryOp::Neg ? "neg" : "not");
            doPrintExpr(*u.operand, d + 1);
            break;
        }
        case Expr::Kind::Binary: {
            const auto& b = static_cast<const BinaryExpr&>(e);
            indent(d); std::printf("bin %s\n", binOpName(b.op));
            doPrintExpr(*b.left, d + 1);
            doPrintExpr(*b.right, d + 1);
            break;
        }
        case Expr::Kind::Group: {
            indent(d); std::printf("group\n");
            doPrintExpr(*static_cast<const GroupExpr&>(e).inner, d + 1);
            break;
        }
        case Expr::Kind::Call: {
            const auto& c = static_cast<const CallExpr&>(e);
            indent(d); std::printf("call %s(%zu args)\n", c.callee.c_str(), c.args.size());
            for (const auto& a : c.args) doPrintExpr(*a, d + 1);
            break;
        }
    }
}

static void doPrintStmtImpl(const Stmt& s, int d) {
    switch (s.kind) {
        case Stmt::Kind::Let: {
            const auto& l = static_cast<const LetStmt&>(s);
            indent(d); std::printf("let %s\n", l.name.c_str());
            if (l.init) doPrintExpr(*l.init, d + 1);
            break;
        }
        case Stmt::Kind::Assign: {
            const auto& a = static_cast<const AssignStmt&>(s);
            indent(d); std::printf("assign %s\n", a.name.c_str());
            doPrintExpr(*a.value, d + 1);
            break;
        }
        case Stmt::Kind::Return: {
            indent(d); std::printf("return\n");
            const auto& r = static_cast<const ReturnStmt&>(s);
            if (r.value) doPrintExpr(*r.value, d + 1);
            break;
        }
        case Stmt::Kind::Print: {
            indent(d); std::printf("print\n");
            doPrintExpr(*static_cast<const PrintStmt&>(s).expr, d + 1);
            break;
        }
        case Stmt::Kind::If: {
            const auto& i = static_cast<const IfStmt&>(s);
            indent(d); std::printf("if\n");
            doPrintExpr(*i.cond, d + 1);
            indent(d); std::printf("then:\n");
            doPrintStmtImpl(*i.thenBranch, d + 1);
            if (i.elseBranch) { indent(d); std::printf("else:\n"); doPrintStmtImpl(*i.elseBranch, d + 1); }
            break;
        }
        case Stmt::Kind::While: {
            const auto& w = static_cast<const WhileStmt&>(s);
            indent(d); std::printf("while\n");
            doPrintExpr(*w.cond, d + 1);
            indent(d); std::printf("body:\n");
            doPrintStmtImpl(*w.body, d + 1);
            break;
        }
        case Stmt::Kind::Block: {
            indent(d); std::printf("block\n");
            for (const auto& st : static_cast<const BlockStmt&>(s).stmts) doPrintStmtImpl(*st, d + 1);
            break;
        }
        case Stmt::Kind::Expr: {
            indent(d); std::printf("expr-stmt\n");
            doPrintExpr(*static_cast<const ExprStmt&>(s).expr, d + 1);
            break;
        }
        case Stmt::Kind::FuncDecl: {
            const auto& f = static_cast<const FuncDecl&>(s);
            indent(d); std::printf("func %s(", f.name.c_str());
            for (size_t i = 0; i < f.params.size(); i++) {
                if (i) std::printf(", ");
                std::printf("%s", f.params[i].c_str());
            }
            std::printf(")\n");
            doPrintStmtImpl(*f.body, d + 1);
            break;
        }
    }
}

void doPrintStmt(const Stmt& s, int depth) { doPrintStmtImpl(s, depth); }

void printAst(const Program& program) {
    std::printf("==== AST ====\n");
    for (const auto& f : program.funcs) { doPrintStmtImpl(*f, 0); std::printf("\n"); }
    for (const auto& s : program.stmts) { doPrintStmtImpl(*s, 0); std::printf("\n"); }
}

} // namespace mini