# LL分析法：自顶向下解析的理论与实践

## LL分析的核心思想

LL分析是一种自顶向下的语法分析方法，"LL"的含义是：从左到右扫描输入，进行最左推导。

### 基本原理
```
LL(1): Left-to-right scan, Leftmost derivation, 1 symbol lookahead

分析过程:
1. 从开始符号出发
2. 按照最左推导顺序
3. 根据当前输入符号选择产生式
4. 逐步匹配输入串
```

**核心哲学**: **预测性分析** - 基于当前信息预测后续结构

## LL分析的理论基础

### 1. FIRST集的计算
**定义**: 对于符号串α，FIRST(α)是可以从α推导出的所有串的首符号集合。

**形式化定义**:
```
1. 如果 X 是终结符，则 FIRST(X) = {X}
2. 如果 X → ε 是产生式，则 ε ∈ FIRST(X)
3. 如果 X 是非终结符:
   a. 对于所有产生式 X → Y₁Y₂...Yₖ
      将 FIRST(Y₁) - {ε} 加入 FIRST(X)
   b. 如果 ε ∈ FIRST(Yᵢ) 对于所有 i < j
      将 FIRST(Yⱼ) - {ε} 加入 FIRST(X)
   c. 如果 ε ∈ FIRST(Yᵢ) 对所有 i = 1..k
      则 ε ∈ FIRST(X)
```

**计算算法**:
```cpp
class FirstSetCalculator {
private:
    Grammar& grammar_;
    std::map<Symbol, std::set<Terminal>> firstSets_;

public:
    void calculate() {
        bool changed = true;

        while (changed) {
            changed = false;

            for (auto& nonTerminal : grammar_.getNonTerminals()) {
                auto& productions = grammar_.getProductions(nonTerminal);

                for (auto& production : productions) {
                    // X → Y₁Y₂...Yₖ
                    auto result = computeFirst(production.getBody());

                    for (auto terminal : result) {
                        if (firstSets_[nonTerminal].insert(terminal).second) {
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    std::set<Terminal> computeFirst(const std::vector<Symbol>& symbols) {
        std::set<Terminal> result;
        bool allHaveEpsilon = true;

        for (auto& symbol : symbols) {
            auto first = getFirst(symbol);
            result.insert(first.begin(), first.end());

            if (first.find(Terminal::EPSILON) == first.end()) {
                allHaveEpsilon = false;
                break;
            }
        }

        if (allHaveEpsilon) {
            result.insert(Terminal::EPSILON);
        }

        return result;
    }
};
```

### 2. FOLLOW集的计算
**定义**: 对于非终结符A，FOLLOW(A)是在某些句子中紧跟在A后面的终结符集合。

**形式化定义**:
```
1. $ ∈ FOLLOW(S) (S是开始符号)
2. 如果存在产生式 A → αBβ:
   则 FIRST(β) - {ε} ⊆ FOLLOW(B)
3. 如果存在产生式 A → αB 或 A → αBβ，其中 ε ∈ FIRST(β):
   则 FOLLOW(A) ⊆ FOLLOW(B)
```

**计算算法**:
```cpp
class FollowSetCalculator {
private:
    Grammar& grammar_;
    std::map<Symbol, std::set<Terminal>> followSets_;

public:
    void calculate() {
        // 初始化: 将$加入开始符号的FOLLOW集
        followSets_[grammar_.getStartSymbol()].insert(Terminal::EOF_);

        bool changed = true;
        while (changed) {
            changed = false;

            for (auto& production : grammar_.getAllProductions()) {
                auto head = production.getHead();
                auto body = production.getBody();

                // 检查产生式 A → αBβ
                for (size_t i = 0; i < body.size(); i++) {
                    if (!body[i].isNonTerminal()) continue;

                    auto B = body[i];

                    // 计算β的FIRST集
                    std::vector<Symbol> beta(body.begin() + i + 1, body.end());
                    auto firstBeta = computeFirst(beta);

                    // 将FIRST(β) - {ε}加入FOLLOW(B)
                    for (auto terminal : firstBeta) {
                        if (terminal != Terminal::EPSILON) {
                            if (followSets_[B].insert(terminal).second) {
                                changed = true;
                            }
                        }
                    }

                    // 如果ε ∈ FIRST(β)，则FOLLOW(A) ⊆ FOLLOW(B)
                    if (firstBeta.find(Terminal::EPSILON) != firstBeta.end() || i == body.size() - 1) {
                        for (auto terminal : followSets_[head]) {
                            if (followSets_[B].insert(terminal).second) {
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
};
```

### 3. LL(1)文法的判定条件
**定理**: 文法G是LL(1)的当且仅当对于G的任意两个不同产生式 A → α | β：

1. **FIRST集不相交**: FIRST(α) ∩ FIRST(β) = ∅
2. ** FOLLOW集不相交**: 如果 ε ∈ FIRST(β)，则 FIRST(α) ∩ FOLLOW(A) = ∅

**检验算法**:
```cpp
class LL1Validator {
private:
    Grammar& grammar_;
    FirstSetCalculator firstCalc_;
    FollowSetCalculator followCalc_;

public:
    bool isLL1() {
        firstCalc_.calculate();
        followCalc_.calculate();

        for (auto& nonTerminal : grammar_.getNonTerminals()) {
            auto& productions = grammar_.getProductions(nonTerminal);

            // 检查每对产生式
            for (size_t i = 0; i < productions.size(); i++) {
                for (size_t j = i + 1; j < productions.size(); j++) {
                    auto& prod1 = productions[i];
                    auto& prod2 = productions[j];

                    auto first1 = firstCalc_.getFirst(prod1.getBody());
                    auto first2 = firstCalc_.getFirst(prod2.getBody());

                    // 条件1: FIRST集不相交
                    std::set<Terminal> intersection;
                    std::set_intersection(first1.begin(), first1.end(),
                                        first2.begin(), first2.end(),
                                        std::inserter(intersection, intersection.begin()));

                    if (!intersection.empty()) {
                        // 如果交集只包含ε，检查条件2
                        if (intersection.size() == 1 &&
                            *intersection.begin() == Terminal::EPSILON) {
                            auto followA = followCalc_.getFollow(nonTerminal);

                            std::set<Terminal> conflict;
                            std::set_intersection(first1.begin(), first1.end(),
                                                followA.begin(), followA.end(),
                                                std::inserter(conflict, conflict.begin()));

                            if (!conflict.empty()) {
                                return false; // 不是LL(1)文法
                            }
                        } else {
                            return false; // 不是LL(1)文法
                        }
                    }
                }
            }
        }

        return true;
    }
};
```

## LL(1)分析表的构造

### 1. 分析表结构
```
         输入符号 (a ∈ Σ ∪ {$})
     id  +  *  (  )  $  ...
   ┌─────────────────────┐
 A │                     │
   │                     │
 B │                     │
   │                     │
...│                     │
   └─────────────────────┘
```

### 2. 构造算法
```cpp
class LL1TableConstructor {
private:
    Grammar& grammar_;
    FirstSetCalculator firstCalc_;
    FollowSetCalculator followCalc_;

public:
    ParseTable construct() {
        ParseTable table;
        firstCalc_.calculate();
        followCalc_.calculate();

        for (auto& production : grammar_.getAllProductions()) {
            auto A = production.getHead();
            auto alpha = production.getBody();

            // 对于FIRST(α)中的每个终结符a
            auto firstAlpha = firstCalc_.getFirst(alpha);

            for (auto terminal : firstAlpha) {
                if (terminal != Terminal::EPSILON) {
                    table[A][terminal] = production;
                }
            }

            // 如果ε ∈ FIRST(α)
            if (firstAlpha.find(Terminal::EPSILON) != firstAlpha.end()) {
                // 对于FOLLOW(A)中的每个终结符b
                auto followA = followCalc_.getFollow(A);

                for (auto terminal : followA) {
                    table[A][terminal] = production;
                }
            }
        }

        return table;
    }
};
```

### 3. 分析表示例
**文法**:
```
E → TE'
E' → +TE' | ε
T → FT'
T' → *FT' | ε
F → (E) | id
```

**FIRST集**:
```
FIRST(E) = {(, id}
FIRST(E') = {+, ε}
FIRST(T) = {(, id}
FIRST(T') = {*, ε}
FIRST(F) = {(, id}
```

**FOLLOW集**:
```
FOLLOW(E) = {), $}
FOLLOW(E') = {), $}
FOLLOW(T) = {+, ), $}
FOLLOW(T') = {+, ), $}
FOLLOW(F) = {*, +, ), $}
```

**分析表**:
```
         id      +       *       (       )       $
   ┌───────────────────────────────────────────┐
 E  │  E→TE'          E→TE'                           │
   │                                           │
E'  │         E'→+TE'          E'→ε           E'→ε    │
   │                                           │
 T  │  T→FT'          T→FT'                           │
   │                                           │
T'  │         T'→ε    T'→*FT'         T'→ε     T'→ε    │
   │                                           │
 F  │  F→id                   F→(E)                   │
   │                                           │
   └───────────────────────────────────────────┘
```

## 预测分析法实现

### 1. 非递归预测分析
```cpp
class PredictiveParser {
private:
    ParseTable& table_;
    std::stack<Symbol> stack_;
    Lexer& lexer_;
    Token currentToken_;

public:
    void parse() {
        currentToken_ = lexer_.getNextToken();
        stack_.push(Terminal::EOF_);
        stack_.push(grammar_.getStartSymbol());

        while (!stack_.empty()) {
            Symbol X = stack_.top();
            stack_.pop();

            if (X.isTerminal()) {
                if (X == currentToken_.type) {
                    currentToken_ = lexer_.getNextToken();
                } else {
                    error("Expected terminal mismatch");
                }
            } else if (X.isNonTerminal()) {
                auto production = table_[X][currentToken_.type];

                if (production.isEmpty()) {
                    error("No production for this input");
                }

                // 将产生式右部逆序压入栈
                auto body = production.getBody();
                for (auto it = body.rbegin(); it != body.rend(); ++it) {
                    if (*it != Terminal::EPSILON) {
                        stack_.push(*it);
                    }
                }
            }
        }

        if (currentToken_.type != TokenType::EOF_) {
            error("Extra input after parsing");
        }
    }
};
```

### 2. 递归下降分析
```cpp
class RecursiveDescentParser {
private:
    Lexer& lexer_;
    Token currentToken_;

public:
    void parse() {
        currentToken_ = lexer_.getNextToken();
        parseE();

        if (currentToken_.type != TokenType::EOF_) {
            throw ParseException("Unexpected end of input");
        }
    }

private:
    // E → TE'
    std::unique_ptr<Expr> parseE() {
        auto node = std::make_unique<BinaryExpr>();
        node->setLeft(parseT());
        node->setOp(parseEPrime());

        return node;
    }

    // E' → +TE' | ε
    std::optional<TokenType> parseEPrime() {
        if (currentToken_.type == TokenType::PLUS) {
            advance();
            parseT();
            parseEPrime();
            return TokenType::PLUS;
        }
        return std::nullopt; // ε
    }

    // T → FT'
    std::unique_ptr<Expr> parseT() {
        auto node = std::make_unique<BinaryExpr>();
        node->setLeft(parseF());
        node->setOp(parseTPrime());

        return node;
    }

    // T' → *FT' | ε
    std::optional<TokenType> parseTPrime() {
        if (currentToken_.type == TokenType::MULTIPLY) {
            advance();
            parseF();
            parseTPrime();
            return TokenType::MULTIPLY;
        }
        return std::nullopt; // ε
    }

    // F → (E) | id
    std::unique_ptr<Expr> parseF() {
        if (currentToken_.type == TokenType::LPAREN) {
            advance();
            auto expr = parseE();
            expect(TokenType::RPAREN);
            return expr;
        } else if (currentToken_.type == TokenType::IDENTIFIER) {
            auto name = currentToken_.lexeme;
            advance();
            return std::make_unique<IdentifierExpr>(name);
        } else {
            throw ParseException("Expected '(' or identifier");
        }
    }

    void advance() {
        currentToken_ = lexer_.getNextToken();
    }

    void expect(TokenType type) {
        if (currentToken_.type == type) {
            advance();
        } else {
            throw ParseException("Unexpected token");
        }
    }
};
```

## 文法变换技术

### 1. 消除左递归
**直接左递归消除**:
```
原始文法:
E → E + T | E - T | T

消除左递归:
E  → TE'
E' → +TE' | -TE' | ε
```

**算法**:
```cpp
class LeftRecursionEliminator {
public:
    Grammar eliminate(Grammar& grammar) {
        Grammar result;

        for (auto& A : grammar.getNonTerminals()) {
            auto productions = grammar.getProductions(A);

            // 分离左递归和非左递归产生式
            std::vector<Production> recursive;
            std::vector<Production> nonRecursive;

            for (auto& production : productions) {
                if (production.isLeftRecursive(A)) {
                    recursive.push_back(production);
                } else {
                    nonRecursive.push_back(production);
                }
            }

            if (recursive.empty()) {
                // 没有左递归，保持原样
                result.addProductions(A, productions);
            } else {
                // 创建新的非终结符A'
                Symbol Aprime = A + "'";

                // 添加 A → αA' (α是非左递归产生式体)
                for (auto& prod : nonRecursive) {
                    auto body = prod.getBody();
                    body.push_back(Aprime);
                    result.addProduction(A, body);
                }

                // 添加 A' → βA' | ε (β是左递归产生式体去掉A)
                for (auto& prod : recursive) {
                    auto body = prod.getBody();
                    body.erase(body.begin()); // 去掉左递归的A
                    body.push_back(Aprime);
                    result.addProduction(Aprime, body);
                }

                // 添加 ε 产生式
                result.addProduction(Aprime, {Terminal::EPSILON});
            }
        }

        return result;
    }
};
```

### 2. 提取左因子
**问题识别**:
```
原始文法:
A → αβ₁ | αβ₂
```

**左因子提取**:
```
变换后:
A → αA'
A' → β₁ | β₂
```

**算法**:
```cpp
class LeftFactoring {
public:
    Grammar factor(Grammar& grammar) {
        Grammar result;
        bool changed = true;

        while (changed) {
            changed = false;

            for (auto& A : grammar.getNonTerminals()) {
                auto productions = grammar.getProductions(A);

                // 寻找公共前缀
                for (size_t i = 0; i < productions.size(); i++) {
                    for (size_t j = i + 1; j < productions.size(); j++) {
                        auto prefix = findCommonPrefix(
                            productions[i].getBody(),
                            productions[j].getBody()
                        );

                        if (!prefix.empty()) {
                            // 提取左因子
                            auto Aprime = A + "'";
                            auto remaining1 = productions[i].getBody();
                            auto remaining2 = productions[j].getBody();

                            // 去掉公共前缀
                            remaining1.erase(remaining1.begin(),
                                           remaining1.begin() + prefix.size());
                            remaining2.erase(remaining2.begin(),
                                           remaining2.begin() + prefix.size());

                            // 创建新产生式
                            std::vector<Symbol> newBody = prefix;
                            newBody.push_back(Aprime);

                            result.addProduction(A, newBody);
                            result.addProduction(Aprime, remaining1);
                            result.addProduction(Aprime, remaining2);

                            changed = true;
                            break;
                        }
                    }

                    if (changed) break;
                }
            }
        }

        return result;
    }

private:
    std::vector<Symbol> findCommonPrefix(
        const std::vector<Symbol>& seq1,
        const std::vector<Symbol>& seq2
    ) {
        std::vector<Symbol> prefix;
        size_t minLen = std::min(seq1.size(), seq2.size());

        for (size_t i = 0; i < minLen; i++) {
            if (seq1[i] == seq2[i]) {
                prefix.push_back(seq1[i]);
            } else {
                break;
            }
        }

        return prefix;
    }
};
```

## 错误处理与恢复

### 1. 错误检测
```cpp
class ErrorDetection {
public:
    enum class ErrorType {
        NO_MATCH,           // 分析表中无对应条目
        TERMINAL_MISMATCH,  // 终结符不匹配
        UNEXPECTED_EOF,     // 意外遇到文件结束
        STACK_UNDERFLOW     // 栈下溢
    };

    struct Error {
        ErrorType type;
        Symbol expected;
        Symbol found;
        size_t line;
        size_t column;

        std::string format() const {
            return fmt::format("Error at line {}, column {}: Expected {}, but found {}",
                              line, column, expected, found);
        }
    };

    std::optional<Error> detectError(
        const Symbol& stackTop,
        const Token& currentToken,
        const ParseTable& table
    ) {
        if (stackTop.isTerminal()) {
            if (stackTop != currentToken.type) {
                return Error{
                    ErrorType::TERMINAL_MISMATCH,
                    stackTop,
                    currentToken.type,
                    currentToken.line,
                    currentToken.column
                };
            }
        } else {
            if (!table.hasEntry(stackTop, currentToken.type)) {
                return Error{
                    ErrorType::NO_MATCH,
                    stackTop,
                    currentToken.type,
                    currentToken.line,
                    currentToken.column
                };
            }
        }

        return std::nullopt;
    }
};
```

### 2. 错误恢复策略
```cpp
class ErrorRecovery {
public:
    // 恐慌模式恢复
    void panicMode(Parser& parser, const std::set<TokenType>& syncTokens) {
        while (!parser.isAtEnd()) {
            Token current = parser.currentToken();

            if (syncTokens.find(current.type) != syncTokens.end()) {
                return; // 找到同步点
            }

            parser.advance();
        }
    }

    // 短语级恢复
    bool phraseLevel(Parser& parser, const Error& error) {
        // 尝试插入缺失的符号
        if (canInsert(parser, error.expected)) {
            parser.insertSymbol(error.expected);
            return true;
        }

        // 尝试删除当前符号
        if (canSkip(parser, error.found)) {
            parser.advance();
            return true;
        }

        // 尝试替换符号
        if (canReplace(parser, error.found, error.expected)) {
            parser.replaceSymbol(error.expected);
            return true;
        }

        return false;
    }

    // 全局纠正
    Program globalCorrection(const Program& errorProgram) {
        // 使用编辑距离算法寻找最接近的正确程序
        auto candidates = generateCandidates(errorProgram);
        return findClosest(errorProgram, candidates);
    }
};
```

## LL分析的局限性与扩展

### 1. LL(1)的局限性
**无法处理的文法**:
- **左递归文法**: 直接或间接左递归
- **公共前缀文法**: 没有提取左因子
- **需要更多前瞻的文法**: 单个符号不足以做出决策

**示例**:
```
// 歧义文法
S → if E then S
  | if E then S else S
  | other

// 需要更多前瞻
E → E + E | E * E | id
```

### 2. LL(k)分析
**扩展**: 使用k个符号的前瞻来做出决策

**分析表**: M[A, a₁a₂...aₖ]

**复杂度**: 分析表大小为 O(|G| × |Σ|ᵏ)

### 3. LL(*)分析 (ANTLR)
**思想**: 使用自适应的前瞻预测

**实现**:
```cpp
class LLStarParser {
private:
    std::map<std::pair<Symbol, std::vector<Token>>, Production> cache_;

public:
    std::optional<Production> predict(
        Symbol nonTerminal,
        const std::vector<Token>& lookahead
    ) {
        // 检查缓存
        auto key = std::make_pair(nonTerminal, lookahead);
        if (cache_.find(key) != cache_.end()) {
            return cache_[key];
        }

        // 尝试所有产生式
        auto productions = grammar_.getProductions(nonTerminal);

        for (auto& production : productions) {
            if (matches(production, lookahead)) {
                cache_[key] = production;
                return production;
            }
        }

        return std::nullopt;
    }

private:
    bool matches(const Production& production,
                const std::vector<Token>& lookahead) {
        // 模拟推导，检查是否匹配
        std::vector<Token> remaining = lookahead;
        std::vector<Symbol> stack = production.getBody();

        while (!stack.empty() && !remaining.empty()) {
            Symbol symbol = stack.back();
            stack.pop_back();

            if (symbol.isTerminal()) {
                if (symbol != remaining[0].type) {
                    return false;
                }
                remaining.erase(remaining.begin());
            } else {
                // 需要递归预测...
            }
        }

        return stack.empty();
    }
};
```

## 实践建议与最佳实践

### 1. 文法设计原则
- **避免左递归**: 优先使用右递归
- **提取左因子**: 减少歧义性
- **保持简单**: 复杂的文法难以维护
- **测试覆盖**: 为每个语法结构编写测试

### 2. 调试技巧
```cpp
class ParserDebugger {
public:
    void traceExecution(Parser& parser) {
        std::cout << "Parse trace:\n";

        while (!parser.isAtEnd()) {
            auto stack = parser.getStack();
            auto current = parser.currentToken();

            std::cout << "Stack: ";
            for (auto& symbol : stack) {
                std::cout << symbol << " ";
            }
            std::cout << "\n";

            std::cout << "Input: " << current << "\n";
            std::cout << "---\n";

            parser.step();
        }
    }

    void visualizeTable(const ParseTable& table) {
        for (auto& [row, columns] : table) {
            std::cout << row << ":\n";
            for (auto& [col, production] : columns) {
                std::cout << "  " << col << " -> " << production << "\n";
            }
        }
    }
};
```

### 3. 性能优化
- **分析表压缩**: 减少内存占用
- **内联展开**: 减少函数调用开销
- **缓存结果**: 避免重复计算
- **预计算FIRST/FOLLOW**: 加速分析过程

## 总结

LL分析法体现了自顶向下分析的哲学思想：

1. **预测性思维**: 基于有限信息做出决策
2. **递归结构**: 利用递归处理层次结构
3. **形式化基础**: 基于坚实的数学理论
4. **工程实用性**: 广泛应用于实际编译器

虽然LL分析有一定的局限性，但它的思想清晰，实现直观，是理解语法分析的理想起点。通过学习LL分析，可以深入理解编译原理的核心概念，为学习更复杂的分析方法奠定基础。