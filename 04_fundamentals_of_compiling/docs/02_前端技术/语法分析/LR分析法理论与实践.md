# LR分析法：自底向上解析的理论与实践

## LR分析的核心思想

LR分析是一种自底向上的语法分析方法，"LR"的含义是：从左到右扫描输入，进行最右推导的逆过程。

### 基本原理
```
LR(k): Left-to-right scan, Rightmost derivation in reverse, k symbols lookahead

分析过程:
1. 从输入串出发
2. 按照最右推导的逆过程(最左归约)
3. 使用栈存储已处理部分
4. 根据栈状态和输入符号决定动作
```

**核心哲学**: **移进-归约策略** - 逐步构建语法树，识别可归约串

## LR分析的理论基础

### 1. 规范归约与句柄
**最右推导**: 总是替换最右边的非终结符
```
E ⇒ E + T ⇒ E + T * F ⇒ E + T * id ⇒ E + id * id ⇒ id + id * id
```

**规范归约**: 最右推导的逆过程
```
id + id * id
⇒ E + id * id    (使用 F → id)
⇒ E + T * id    (使用 T → F)
⇒ E + T * F     (使用 F → id)
⇒ E + T         (使用 T → T * F)
⇒ E             (使用 E → E + T)
```

**句柄**: 在规范归约的每一步，被归约的子串称为句柄

### 2. LR分析器结构
```
┌─────────────────────────────────────┐
│         LR分析器结构                │
├─────────────────────────────────────┤
│  输入缓冲区: id + id * id $         │
│                   ↑                 │
│  栈: s₀ s₁ ... sₘ X₁ X₂ ... Xₘ      │
│       ↑状态      ↑符号              │
├─────────────────────────────────────┤
│  ACTION表: 移进/归约/接受/错误      │
│  GOTO表:  状态转移                  │
└─────────────────────────────────────┘
```

### 3. LR分析器的配置
**配置**: (栈内容, 剩余输入)
```
初始配置: ([s₀], w₁w₂...wₙ$)
中间配置: ([s₀s₁...sₘ, X₁X₂...Xₘ], aᵢaᵢ₊₁...wₙ$)
接受配置: ([s₀s₁...sₘ, X₁X₂...Xₘ = S], $)
```

**分析动作**:
- **移进(sₘ, aᵢ)**: 将(sₘ, aᵢ)的ACTION转移到新状态sⱼ
- **归约(sₘ, A → β)**: 弹出|β|个符号和状态，将A压栈
- **接受**: 分析成功
- **报错**: 发现语法错误

## LR(0)项目集与规范族

### 1. LR(0)项目
**定义**: 在产生式右部添加位置标记
```
A → α·β  (点在α和β之间)
```

**项目类型**:
- **完备项目**: A → α· (点在末尾)
- **非完备项目**: A → α·aβ (点不在末尾)

### 2. LR(0)项目集闭包
```cpp
class LR0ItemSet {
private:
    std::set<Item> items_;

public:
    static LR0ItemSet closure(const std::set<Item>& kernel) {
        LR0ItemSet result;
        result.items_ = kernel;

        bool changed = true;
        while (changed) {
            changed = false;

            // 临时集合存储新项目
            std::set<Item> newItems;

            for (const auto& item : result.items_) {
                // A → α·Bβ 形式的项目
                if (item.hasNextSymbol() && item.nextSymbol().isNonTerminal()) {
                    auto B = item.nextSymbol();

                    // 对于B的每个产生式 B → γ
                    for (const auto& production : grammar_.getProductions(B)) {
                        Item newItem(production, 0); // B → ·γ

                        if (result.items_.find(newItem) == result.items_.end()) {
                            newItems.insert(newItem);
                            changed = true;
                        }
                    }
                }
            }

            result.items_.insert(newItems.begin(), newItems.end());
        }

        return result;
    }

    LR0ItemSet goto(Symbol X) const {
        std::set<Item> kernel;

        for (const auto& item : items_) {
            if (item.hasNextSymbol() && item.nextSymbol() == X) {
                // A → α·Xβ ⇒ A → αX·β
                kernel.insert(item.advance());
            }
        }

        return closure(kernel);
    }
};
```

### 3. LR(0)项目集规范族
```cpp
class LR0CanonicalCollection {
private:
    Grammar grammar_;
    std::vector<LR0ItemSet> itemSets_;
    std::map<std::pair<size_t, Symbol>, size_t> gotoTable_;

public:
    void build() {
        // 初始项目集: S' → ·S
        Item startItem(grammar_.getAugmentedStartProduction(), 0);
        LR0ItemSet initialSet = LR0ItemSet::closure({startItem});

        std::queue<LR0ItemSet> worklist;
        worklist.push(initialSet);
        itemSets_.push_back(initialSet);

        while (!worklist.empty()) {
            auto currentSet = worklist.front();
            worklist.pop();

            size_t currentIdx = indexOf(currentSet);

            // 对所有可能的符号计算GOTO
            for (auto symbol : grammar_.getAllSymbols()) {
                auto nextSet = currentSet.goto(symbol);

                if (!nextSet.isEmpty()) {
                    size_t nextIdx = indexOf(nextSet);

                    if (nextIdx == itemSets_.size()) {
                        // 新的项目集
                        itemSets_.push_back(nextSet);
                        worklist.push(nextSet);
                    }

                    gotoTable_[{currentIdx, symbol}] = nextIdx;
                }
            }
        }
    }

private:
    size_t indexOf(const LR0ItemSet& itemSet) {
        auto it = std::find(itemSets_.begin(), itemSets_.end(), itemSet);
        if (it != itemSets_.end()) {
            return std::distance(itemSets_.begin(), it);
        }
        return itemSets_.size(); // 表示不存在
    }
};
```

## SLR(1)分析表构造

### 1. SLR(1)的改进思想
**问题**: LR(0)的归约冲突过多
**解决**: 使用FOLLOW集限制归约时机

### 2. SLR(1)分析表构造算法
```cpp
class SLR1TableConstructor {
private:
    Grammar grammar_;
    LR0CanonicalCollection canonicalCollection_;
    FollowSetCalculator followCalc_;

public:
    ParseTable construct() {
        canonicalCollection_.build();
        followCalc_.calculate();

        ParseTable table;
        auto itemSets = canonicalCollection_.getItemSets();

        for (size_t i = 0; i < itemSets.size(); i++) {
            const auto& itemSet = itemSets[i];

            // 处理移进动作
            for (const auto& item : itemSet.getItems()) {
                if (item.hasNextSymbol() && item.nextSymbol().isTerminal()) {
                    auto terminal = item.nextSymbol();
                    size_t j = canonicalCollection_.goto(i, terminal);

                    table.setShift(i, terminal, j);
                }
            }

            // 处理归约动作
            for (const auto& item : itemSet.getItems()) {
                if (item.isCompleteItem()) {
                    // A → α·
                    auto production = item.getProduction();
                    auto A = production.getHead();

                    // 对于FOLLOW(A)中的每个终结符a
                    for (auto terminal : followCalc_.getFollow(A)) {
                        // 如果没有冲突，设置归约动作
                        if (!table.hasAction(i, terminal)) {
                            table.setReduce(i, terminal, production);
                        } else {
                            // 冲突检测
                            handleConflict(i, terminal, production);
                        }
                    }
                }
            }

            // 处理接受动作
            if (itemSet.containsAugmentedStartItem()) {
                table.setAccept(i, Terminal::EOF_);
            }
        }

        return table;
    }

private:
    void handleConflict(size_t state, Terminal terminal, Production production) {
        // 记录冲突信息
        std::cout << "Conflict at state " << state
                  << ", terminal " << terminal << "\n";
    }
};
```

### 3. SLR(1)分析器实现
```cpp
class SLR1Parser {
private:
    ParseTable& table_;
    std::stack<State> stateStack_;
    std::stack<Symbol> symbolStack_;
    Lexer& lexer_;
    Token currentToken_;

public:
    void parse() {
        currentToken_ = lexer_.getNextToken();
        stateStack_.push(0); // 初始状态

        while (true) {
            State state = stateStack_.top();
            Action action = table_.getAction(state, currentToken_.type);

            switch (action.getType()) {
                case ActionType::SHIFT: {
                    stateStack_.push(action.getNextState());
                    symbolStack_.push(currentToken_.type);
                    currentToken_ = lexer_.getNextToken();
                    break;
                }

                case ActionType::REDUCE: {
                    auto production = action.getProduction();
                    size_t productionLength = production.getBody().size();

                    // 弹出|β|个符号和状态
                    for (size_t i = 0; i < productionLength; i++) {
                        stateStack_.pop();
                        symbolStack_.pop();
                    }

                    // 将A压栈
                    State state = stateStack_.top();
                    symbolStack_.push(production.getHead());

                    // 转移到新状态
                    State nextState = table_.getGoto(state, production.getHead());
                    stateStack_.push(nextState);

                    // 输出产生式（可选）
                    std::cout << production << "\n";
                    break;
                }

                case ActionType::ACCEPT: {
                    std::cout << "Accept\n";
                    return;
                }

                case ActionType::ERROR: {
                    error("Syntax error");
                    return;
                }
            }
        }
    }
};
```

## LR(1)分析

### 1. LR(1)项目
**定义**: LR(1)项目是二元组 [A → α·β, a]
- A → α·β: LR(0)项目
- a: 前瞻符号(lookahead)

**意义**: 只有当输入的下一个符号是a时，才能使用该产生式进行归约

### 2. LR(1)项目集闭包
```cpp
class LR1ItemSet {
private:
    std::set<LR1Item> items_;

public:
    static LR1ItemSet closure(const std::set<LR1Item>& kernel) {
        LR1ItemSet result;
        result.items_ = kernel;

        bool changed = true;
        while (changed) {
            changed = false;

            std::set<LR1Item> newItems;

            for (const auto& item : result.items_) {
                // [A → α·Bβ, a]
                if (item.hasNextSymbol() && item.nextSymbol().isNonTerminal()) {
                    auto B = item.nextSymbol();
                    auto a = item.getLookahead();

                    // 计算FIRST(βa)
                    std::vector<Symbol> beta(item.remainingSymbols());
                    beta.push_back(a);
                    auto firstBeta = computeFirst(beta);

                    // 对于B的每个产生式 B → γ
                    for (const auto& production : grammar_.getProductions(B)) {
                        // 对于FIRST(βa)中的每个b
                        for (auto b : firstBeta) {
                            if (b != Terminal::EPSILON) {
                                LR1Item newItem(production, 0, b); // [B → ·γ, b]

                                if (result.items_.find(newItem) == result.items_.end()) {
                                    newItems.insert(newItem);
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }

            result.items_.insert(newItems.begin(), newItems.end());
        }

        return result;
    }
};
```

### 3. LR(1)分析表构造
```cpp
class LR1TableConstructor {
public:
    ParseTable construct() {
        LR1CanonicalCollection canonicalCollection;
        canonicalCollection.build();

        ParseTable table;
        auto itemSets = canonicalCollection.getItemSets();

        for (size_t i = 0; i < itemSets.size(); i++) {
            const auto& itemSet = itemSets[i];

            // 处理移进动作
            for (const auto& item : itemSet.getItems()) {
                if (item.hasNextSymbol() && item.nextSymbol().isTerminal()) {
                    auto terminal = item.nextSymbol();
                    size_t j = canonicalCollection.goto(i, terminal);

                    table.setShift(i, terminal, j);
                }
            }

            // 处理归约动作
            for (const auto& item : itemSet.getItems()) {
                if (item.isCompleteItem()) {
                    // [A → α·, a]
                    auto production = item.getProduction();
                    auto a = item.getLookahead();

                    if (!table.hasAction(i, a)) {
                        table.setReduce(i, a, production);
                    } else {
                        // 冲突处理
                        handleConflict(i, a, production);
                    }
                }
            }

            // 处理接受动作
            if (itemSet.containsAugmentedStartItem()) {
                table.setAccept(i, Terminal::EOF_);
            }
        }

        return table;
    }
};
```

## LALR(1)分析

### 1. LALR(1)的基本思想
**问题**: LR(1)状态数量过多
**解决**: 合并同心集，减少状态数量

**同心集**: LR(1)项目集中，忽略前瞻符号后相同的LR(0)项目集

### 2. LALR(1)构造算法
```cpp
class LALR1Constructor {
public:
    ParseTable construct() {
        // 1. 构造LR(1)项目集规范族
        LR1CanonicalCollection lr1Collection;
        lr1Collection.build();

        // 2. 合并同心集
        auto mergedSets = mergeCoreSets(lr1Collection.getItemSets());

        // 3. 构造LALR(1)分析表
        return constructTable(mergedSets);
    }

private:
    std::vector<LR1ItemSet> mergeCoreSets(
        const std::vector<LR1ItemSet>& lr1Sets
    ) {
        std::vector<LR1ItemSet> result;
        std::map<size_t, size_t> mergeMap; // 原索引 -> 合并后索引

        for (size_t i = 0; i < lr1Sets.size(); i++) {
            auto core = lr1Sets[i].getCore(); // 忽略前瞻符号

            // 查找是否已有相同的核心
            bool merged = false;
            for (size_t j = 0; j < result.size(); j++) {
                if (result[j].getCore() == core) {
                    // 合并前瞻符号
                    result[j].mergeLookaheads(lr1Sets[i]);
                    mergeMap[i] = j;
                    merged = true;
                    break;
                }
            }

            if (!merged) {
                mergeMap[i] = result.size();
                result.push_back(lr1Sets[i]);
            }
        }

        // 重新计算转移
        recomputeGoto(result, mergeMap);

        return result;
    }

    void recomputeGoto(
        std::vector<LR1ItemSet>& itemSets,
        const std::map<size_t, size_t>& mergeMap
    ) {
        // 根据合并映射重新计算状态转移
        // ...
    }
};
```

## LR分析的冲突处理

### 1. 移进-归约冲突
**定义**: 同一状态下，对同一输入符号，既有移进动作又有归约动作

**示例**:
```
状态i包含项目:
E → E·+T
T → T·*F
对输入*: 移进 vs 归约 T → T
```

**解决策略**:
1. **优先级**: 根据运算符优先级决定
2. **结合性**: 根据结合性决定
3. **默认选择**: 优先移进或优先归约

### 2. 归约-归约冲突
**定义**: 同一状态下，对同一输入符号，有多个归约动作

**示例**:
```
状态i包含项目:
E → id
T → id
对输入id: 归约 E → id vs 归约 T → id
```

**解决策略**:
1. **文法修改**: 消除歧义
2. **优先级**: 选择最长匹配
3. **上下文信息**: 使用更多上下文

### 3. 冲突解决实现
```cpp
class ConflictResolver {
private:
    struct PrecedenceRule {
        std::string op;
        int precedence;
        bool isLeftAssociative;
    };

    std::map<std::string, PrecedenceRule> precedenceRules_;

public:
    ActionType resolve(
        const ShiftReduceConflict& conflict,
        const PrecedenceRules& rules
    ) {
        auto shiftOp = conflict.shiftOperator();
        auto reduceOp = conflict.reduceOperator();

        auto shiftPrec = rules.getPrecedence(shiftOp);
        auto reducePrec = rules.getPrecedence(reduceOp);

        if (shiftPrec.precedence > reducePrec.precedence) {
            return ActionType::SHIFT;
        } else if (shiftPrec.precedence < reducePrec.precedence) {
            return ActionType::REDUCE;
        } else {
            // 优先级相同，根据结合性决定
            if (shiftPrec.isLeftAssociative) {
                return ActionType::REDUCE;
            } else {
                return ActionType::SHIFT;
            }
        }
    }
};
```

## LR分析的错误恢复

### 1. 错误检测
```cpp
class LRErrorDetector {
public:
    enum class ErrorType {
        SHIFT_REDUCE_CONFLICT,
        REDUCE_REDUCE_CONFLICT,
        NO_VALID_ACTION,
        UNEXPECTED_TOKEN
    };

    struct Error {
        ErrorType type;
        State state;
        Token token;
        std::string message;

        std::string format() const {
            return fmt::format("Error at state {}, token {}: {}",
                             state, token, message);
        }
    };

    std::optional<Error> detect(
        const Parser& parser,
        const ParseTable& table
    ) {
        auto state = parser.getCurrentState();
        auto token = parser.currentToken();
        auto action = table.getAction(state, token.type);

        if (!action.isValid()) {
            return Error{
                ErrorType::NO_VALID_ACTION,
                state,
                token,
                "No valid action for this token"
            };
        }

        return std::nullopt;
    }
};
```

### 2. 错误恢复策略
```cpp
class LRErrorRecovery {
public:
    // 恐慌模式
    void panicMode(Parser& parser, const std::set<TokenType>& syncTokens) {
        while (!parser.isAtEnd()) {
            Token current = parser.currentToken();

            if (syncTokens.find(current.type) != syncTokens.end()) {
                return;
            }

            parser.advance();
        }
    }

    // 短语级恢复
    bool phraseLevel(Parser& parser, const Error& error) {
        // 尝试插入符号
        if (tryInsert(parser, error)) {
            return true;
        }

        // 尝试删除符号
        if (tryDelete(parser, error)) {
            return true;
        }

        // 尝试替换符号
        if (tryReplace(parser, error)) {
            return true;
        }

        return false;
    }

    // 全局纠正
    Program globalCorrection(const Program& errorProgram) {
        // 使用动态规划寻找编辑距离最小的正确程序
        return findClosestValidProgram(errorProgram);
    }
};
```

## LR分析器的优化技术

### 1. 分析表压缩
```cpp
class ParseTableCompressor {
public:
    // 行压缩
    CompressedTable compressRows(const ParseTable& table) {
        CompressedTable result;

        for (size_t row = 0; row < table.rowCount(); row++) {
            // 找出该行的默认值
            auto defaultValue = findDefaultValue(table, row);

            // 只存储与默认值不同的条目
            std::map<Symbol, Action> sparseRow;

            for (auto& [symbol, action] : table.getRow(row)) {
                if (action != defaultValue) {
                    sparseRow[symbol] = action;
                }
            }

            result.setRow(row, defaultValue, sparseRow);
        }

        return result;
    }

    // 列压缩
    CompressedTable compressColumns(const ParseTable& table) {
        // 类似行压缩，但是按列进行
        // ...
    }

private:
    Action findDefaultValue(const ParseTable& table, size_t row) {
        std::map<Action, int> frequency;

        for (auto& [symbol, action] : table.getRow(row)) {
            frequency[action]++;
        }

        // 返回出现频率最高的动作
        return std::max_element(frequency.begin(), frequency.end())->first;
    }
};
```

### 2. 状态最小化
```cpp
class StateMinimizer {
public:
    std::map<size_t, size_t> minimize(
        const ParseTable& table
    ) {
        // 1. 初始划分: 终态和非终态
        Partition currentPartition = initialPartition(table);

        // 2. 迭代细化
        bool changed = true;
        while (changed) {
            changed = false;

            Partition newPartition = currentPartition;

            for (auto& block : currentPartition.getBlocks()) {
                auto refined = refineBlock(table, block);

                if (refined.size() > 1) {
                    newPartition.split(block, refined);
                    changed = true;
                }
            }

            currentPartition = newPartition;
        }

        // 3. 构建状态映射
        return buildStateMapping(currentPartition);
    }
};
```

## LR分析的实际应用

### 1. Yacc/Bison生成器
**输入**: Yacc格式的文法描述
**输出**: LR分析器的C代码

```yacc
%{
#include <stdio.h>
%}

%token NUM
%left '+' '-'
%left '*' '/'

%%

expr : expr '+' expr
     | expr '-' expr
     | expr '*' expr
     | expr '/' expr
     | '(' expr ')'
     | NUM
     ;

%%
```

### 2. 现代LR分析器
**特点**:
- **LALR(1)**: 平衡功能和效率
- **错误恢复**: 更好的错误诊断
- **调试支持**: 可视化分析过程
- **性能优化**: 表压缩、状态最小化

### 3. LR分析的扩展
**广义LR(GLR)**: 处理非LR文法
```cpp
class GLRParser {
private:
    std::vector<Parser> parsers_; // 并行多个分析器

public:
    Forest parse(const std::vector<Token>& tokens) {
        std::vector<Parser> activeParsers;
        std::vector<ParseTree> results;

        activeParsers.push_back(createParser());

        for (auto token : tokens) {
            std::vector<Parser> nextParsers;

            for (auto& parser : activeParsers) {
                auto actions = parser.getPossibleActions(token);

                for (auto action : actions) {
                    Parser newParser = parser.clone();
                    newParser.applyAction(action);
                    nextParsers.push_back(newParser);
                }
            }

            // 合并相同配置的分析器
            activeParsers = mergeParsers(nextParsers);
        }

        // 收集所有成功的分析结果
        for (auto& parser : activeParsers) {
            if (parser.isAccepted()) {
                results.push_back(parser.getParseTree());
            }
        }

        return Forest(results);
    }
};
```

## 总结

LR分析法是自底向上分析的集大成者：

1. **理论基础**: 基于最右推导和规范归约
2. **分析能力**: 可以处理几乎所有编程语言
3. **实现复杂**: 需要构造大型分析表
4. **工程价值**: 广泛应用于编译器生成工具

LR分析的美妙在于：
- **数学严谨**: 基于形式语言理论
- **功能强大**: 可以处理复杂文法
- **工程实用**: 有成熟的工具支持
- **思想深刻**: 体现了自动机理论的应用

虽然LR分析比LL分析复杂，但它的分析能力更强，是实际编译器项目中的首选。通过学习LR分析，可以深入理解语法分析的本质，掌握编译器前端的核心技术。