// main.cpp —— MiniLang 命令行入口
//
// 用法：
//   mini <文件.mini> [选项]
//     --tokens   打印词法分析的结果（记号流）
//     --ast      打印语法分析的结果（抽象语法树）
//     --ir       打印中间代码（三地址码）
//     --run      解释执行中间代码（默认执行）
//
// 不带选项时默认执行。可叠加，例如：
//   mini demo.mini --tokens --ast --ir --run
// 会完整展示"字符流 → 记号流 → AST → 三地址码 → 执行结果"整条管线。

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "interpreter.h"
#include "ir.h"
#include "lexer.h"
#include "parser.h"

static std::string readFile(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) {
        std::cerr << "无法打开文件: " << path << "\n";
        return "";
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

static void printTokens(const std::vector<mini::Token>& toks) {
    std::printf("==== 记号流 (Tokens) ====\n");
    for (const auto& t : toks) {
        if (t.type == mini::TokenType::END) break;
        std::printf("  行%-3d  %-8s  %s\n", t.line, mini::tokenTypeName(t.type), t.lexeme.c_str());
    }
    std::printf("\n");
}

int main(int argc, char** argv) {
    std::setvbuf(stdout, nullptr, _IONBF, 0); // 阶段输出即时可见，便于观察崩溃位置
    if (argc < 2) {
        std::cerr << "用法: mini <文件.mini> [--tokens] [--ast] [--ir] [--run]\n";
        return 1;
    }

    std::string file = argv[1];
    bool doTokens = false, doAst = false, doIr = false, doRun = false;
    for (int i = 2; i < argc; i++) {
        std::string a = argv[i];
        if (a == "--tokens") doTokens = true;
        else if (a == "--ast") doAst = true;
        else if (a == "--ir") doIr = true;
        else if (a == "--run") doRun = true;
        else { std::cerr << "未知选项: " << a << "\n"; return 1; }
    }
    if (!doTokens && !doAst && !doIr) doRun = true; // 默认执行

    std::string source = readFile(file);
    if (source.empty()) return 1;

    try {
        // —— 词法分析（前端）——
        mini::Lexer lexer(source);
        auto tokens = lexer.tokenize();
        std::string lexerErr;
        for (auto& t : tokens)
            if (t.type == mini::TokenType::ERROR) lexerErr += "\n  行" + std::to_string(t.line) + ": " + t.lexeme;
        if (!lexerErr.empty()) {
            std::cerr << "词法错误:" << lexerErr << "\n";
            return 1;
        }
        if (doTokens) printTokens(tokens);

        // —— 语法分析（前端）——
        mini::Parser parser(tokens);
        auto program = parser.parseProgram();
        if (doAst) mini::printAst(*program);

        // —— 中间代码生成（中端）——
        auto ir = mini::generateIR(*program);
        if (doIr) mini::printIR(*ir);

        // —— 解释执行（语义 + 运行时）——
        if (doRun) {
            mini::Interpreter interpreter(ir.get());
            interpreter.run();
        }
    } catch (const std::exception& e) {
        std::cerr << "编译/运行失败: " << e.what() << "\n";
        return 1;
    }
    return 0;
}