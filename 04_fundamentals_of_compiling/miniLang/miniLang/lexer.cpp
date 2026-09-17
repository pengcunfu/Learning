// lexer.cpp —— 词法分析器实现 + 记号类型打印
//
// 实现要点：
//  - 一次扫描，贪心最长匹配：例如读到 '=' 后再看下一个是不是 '='，是则成 '=='。
//  - 关键字表：先把整串当作标识符拼出来，再查表判断是关键字还是普通变量名。
//  - 位置跟踪（行/列），让所有后续阶段的报错都能指向具体行。

#include "lexer.h"

#include <cctype>
#include <unordered_map>
#include <sstream>

namespace mini {

const char* tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::FUNC:   return "FUNC";
        case TokenType::LET:    return "LET";
        case TokenType::IF:     return "IF";
        case TokenType::ELSE:   return "ELSE";
        case TokenType::WHILE:  return "WHILE";
        case TokenType::PRINT:  return "PRINT";
        case TokenType::RETURN: return "RETURN";
        case TokenType::TRUE:   return "TRUE";
        case TokenType::FALSE:  return "FALSE";
        case TokenType::IDENT:  return "IDENT";
        case TokenType::INT_LITERAL:    return "INT";
        case TokenType::FLOAT_LITERAL:  return "FLOAT";
        case TokenType::STRING_LITERAL: return "STRING";
        case TokenType::PLUS:   return "+";
        case TokenType::MINUS:  return "-";
        case TokenType::STAR:   return "*";
        case TokenType::SLASH:  return "/";
        case TokenType::PERCENT:return "%";
        case TokenType::EQ:     return "==";
        case TokenType::NEQ:    return "!=";
        case TokenType::LT:     return "<";
        case TokenType::LE:     return "<=";
        case TokenType::GT:     return ">";
        case TokenType::GE:     return ">=";
        case TokenType::AND:    return "&&";
        case TokenType::OR:     return "||";
        case TokenType::NOT:    return "!";
        case TokenType::ASSIGN: return "=";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::SEMI:   return ";";
        case TokenType::COMMA:  return ",";
        case TokenType::END:    return "<EOF>";
        case TokenType::ERROR:  return "<ERROR>";
    }
    return "<unknown>";
}

TokenType keywordType(const std::string& text) {
    static const std::unordered_map<std::string, TokenType> kw = {
        {"func", TokenType::FUNC},
        {"let", TokenType::LET},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"print", TokenType::PRINT},
        {"return", TokenType::RETURN},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
    };
    auto it = kw.find(text);
    return it == kw.end() ? TokenType::IDENT : it->second;
}

Lexer::Lexer(std::string source) : src_(std::move(source)) {}

bool Lexer::atEnd() const { return pos_ >= static_cast<int>(src_.size()); }

char Lexer::peek(int ahead) const {
    int p = pos_ + ahead;
    return (p >= 0 && p < static_cast<int>(src_.size())) ? src_[p] : '\0';
}

char Lexer::advance() {
    char c = src_[pos_++];
    if (c == '\n') { line_++; col_ = 1; }
    else col_++;
    return c;
}

void Lexer::skipWhitespaceAndComments() {
    for (;;) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { advance(); }
        else if (c == '/') {
            // 只处理行注释 //，不处理 '//' 出现在字符串里的情况（字符串由单独分支消费）
            if (peek(1) == '/') {
                while (!atEnd() && peek() != '\n') advance(); // 略过整行
            } else {
                return; // 是真正的除号
            }
        } else { return; }
    }
}

bool Lexer::match(char expected) {
    if (atEnd() || peek() != expected) return false;
    advance();
    return true;
}

Token Lexer::makeToken(TokenType type, int startCol, std::string text) {
    Token t;
    t.type = type;
    t.lexeme = std::move(text);
    t.line = line_;
    t.col = startCol;
    return t;
}

Token Lexer::makeError(std::string msg, int startCol) {
    Token t = makeToken(TokenType::ERROR, startCol, std::move(msg));
    return t;
}

Token Lexer::scanNumber(int startCol) {
    std::string text;
    bool isFloat = false;
    while (std::isdigit(static_cast<unsigned char>(peek()))) text += advance();
    if (peek() == '.' && std::isdigit(static_cast<unsigned char>(peek(1)))) {
        isFloat = true;
        text += advance(); // '.'
        while (std::isdigit(static_cast<unsigned char>(peek()))) text += advance();
    }
    Token t = makeToken(isFloat ? TokenType::FLOAT_LITERAL : TokenType::INT_LITERAL,
                        startCol, text);
    if (isFloat) t.float_value = std::stod(text);
    else t.int_value = std::stoll(text);
    return t;
}

Token Lexer::scanString(int startCol) {
    advance(); // 消费开头的 '"'
    std::string value;
    while (!atEnd() && peek() != '"') {
        char c = advance();
        if (c == '\\') {
            char e = advance();
            if (e == 'n') value += '\n';
            else if (e == 't') value += '\t';
            else if (e == '\\') value += '\\';
            else if (e == '"') value += '"';
            else { value += e; }
        } else {
            value += c;
        }
    }
    if (atEnd() && peek() != '"') {
        return makeError("未终止的字符串", startCol);
    }
    advance(); // 消费结尾的 '"'
    Token t = makeToken(TokenType::STRING_LITERAL, startCol, "\"" + value + "\"");
    t.string_value = value;
    return t;
}

Token Lexer::scanIdentifier(int startCol) {
    std::string text;
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
        text += advance();
    }
    TokenType type = keywordType(text); // 关键字 vs 标识符
    return makeToken(type, startCol, text);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    for (;;) {
        skipWhitespaceAndComments();
        int startCol = col_;
        if (atEnd()) { tokens.push_back(makeToken(TokenType::END, startCol, "")); break; }

        char c = peek();
        // 标识符 / 关键字
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(scanIdentifier(startCol));
            continue;
        }
        // 数字
        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(scanNumber(startCol));
            continue;
        }
        // 字符串
        if (c == '"') { tokens.push_back(scanString(startCol)); continue; }

        // 运算符 / 分隔符（最长匹配：单字符不够时贪心读第二个）
        char consumed = advance();
        TokenType type;
        switch (consumed) {
            case '+': type = TokenType::PLUS; break;
            case '-': type = TokenType::MINUS; break;
            case '*': type = TokenType::STAR; break;
            case '=':
                type = (match('=')) ? TokenType::EQ : TokenType::ASSIGN; break;
            case '!':
                type = (match('=')) ? TokenType::NEQ : TokenType::NOT; break;
            case '<':
                type = (match('=')) ? TokenType::LE : TokenType::LT; break;
            case '>':
                type = (match('=')) ? TokenType::GE : TokenType::GT; break;
            case '&':
                type = (match('&')) ? TokenType::AND : TokenType::ERROR; break;
            case '|':
                type = (match('|')) ? TokenType::OR : TokenType::ERROR; break;
            case '/': type = TokenType::SLASH; break;   // 注释已在上层跳过
            case '%': type = TokenType::PERCENT; break;
            case '(': type = TokenType::LPAREN; break;
            case ')': type = TokenType::RPAREN; break;
            case '{': type = TokenType::LBRACE; break;
            case '}': type = TokenType::RBRACE; break;
            case ';': type = TokenType::SEMI; break;
            case ',': type = TokenType::COMMA; break;
            default:  type = TokenType::ERROR; break;
        }
        tokens.push_back(makeToken(type, startCol, std::string(1, consumed)));
        if (type == TokenType::ERROR) {
            break; // 遇到无法识别的字符，停止并可报告
        }
    }
    return tokens;
}

} // namespace mini