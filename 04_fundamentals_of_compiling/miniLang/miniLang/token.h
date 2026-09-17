// token.h —— 词法记号（Token）定义
//
// 对应编译原理概念：词法单元 / 终结符（terminal）。
// 源程序的每一个最小单元（关键字、标识符、字面量、运算符、分隔符）
// 都用一个 Token 表示。语法分析器只关心 Token 的类型序列，而不关心字符本身，
// 这正是"字符流 → 记号流"这一抽象的第一步。

#pragma once

#include <string>

namespace mini {

// 记号类型。每种类型对应词法文法中的一个"终结符"。
enum class TokenType {
    // 关键字（保留字）
    FUNC,   // func
    LET,    // let
    IF,     // if
    ELSE,   // else
    WHILE,  // while
    PRINT,  // print
    RETURN, // return
    TRUE,   // true
    FALSE,  // false

    // 字面量和标识符
    IDENT,          // 标识符/变量名/函数名
    INT_LITERAL,    // 整数字面量，如 123
    FLOAT_LITERAL,  // 浮点字面量，如 3.14
    STRING_LITERAL, // 字符串字面量，如 "hello"

    // 运算符
    PLUS,     // +
    MINUS,    // -
    STAR,     // *
    SLASH,    // /
    PERCENT,  // %
    EQ,       // ==
    NEQ,      // !=
    LT,       // <
    LE,       // <=
    GT,       // >
    GE,       // >=
    AND,      // &&
    OR,       // ||
    NOT,      // !
    ASSIGN,   // =

    // 分隔符
    LPAREN, RPAREN, LBRACE, RBRACE,
    SEMI,    // ;
    COMMA,   // ,
    END,     // 输入结束（EOF）
    ERROR    // 无法识别的字符
};

// 一个词法记号：类型 + 原文(lexeme) + 位置 + 数值/字符串附加上下文。
struct Token {
    TokenType type = TokenType::ERROR;
    std::string lexeme;      // 记号的源文本（对字符串字面量含括号，方便调试）
    int line = 0;            // 行号
    int col = 0;             // 列号

    // 字面量对应的实际值（供解释器直接使用，省去二次解析）
    long long int_value = 0;
    double float_value = 0.0;
    std::string string_value;

    bool is(TokenType t) const { return type == t; }
};

// 把类型打印成可读字符串（用于 --tokens / 报错）。
const char* tokenTypeName(TokenType t);

} // namespace mini