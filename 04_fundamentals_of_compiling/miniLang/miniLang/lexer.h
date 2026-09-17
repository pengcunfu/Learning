// lexer.h —— 词法分析器（扫描器）声明
//
// 对应编译原理概念：词法分析（Lexical Analysis）。
// 任务：把原始字符流，按词法规则切成一个个 Token 记号流。
// 这里采用"手工构造扫描器"的方式（相比用 Flex 自动生成），
// 逻辑直白、便于讲解；本质上是把有限自动机的状态转移手工写成了 switch。

#pragma once

#include <string>
#include <vector>

#include "token.h"

namespace mini {

class Lexer {
public:
    explicit Lexer(std::string source);

    // 一次性分词，返回完整记号流（末尾含一个 END 记号）。
    std::vector<Token> tokenize();

private:
    std::string src_;
    int pos_ = 0; // 当前读取位置
    int line_ = 1;
    int col_ = 1;

    bool atEnd() const;
    char peek(int ahead = 0) const; // 前看字符，不消费
    char advance();                 // 取一个字符并前进，返回该字符
    void skipWhitespaceAndComments();

    bool match(char expected); // 若下一个是 expected 则消费并返回 true
    Token makeToken(TokenType type, int startCol, std::string text);
    Token makeError(std::string msg, int startCol);

    Token scanNumber(int startCol);
    Token scanString(int startCol);
    Token scanIdentifier(int startCol);
};

// 关键字表查询：若 text 是关键字返回对应类型，否则返回 IDENT。
TokenType keywordType(const std::string& text);

} // namespace mini