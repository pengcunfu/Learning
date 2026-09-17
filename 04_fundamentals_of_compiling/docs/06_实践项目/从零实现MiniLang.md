# 从零实现一门简易语言 MiniLang

> 本文用一门真正能跑起来的微型语言 **MiniLang**，把编译原理的每个阶段依次走一遍。
> 每一节都回答三个问题：**这一步要解决什么问题？核心概念是什么？在 MiniLang 代码里长什么样？**
> 所有代码与输出都来自仓库 `fundamentals_of_compiling/miniLang`，可随时用一条命令复现。

---

## 0. 一句话看完整条管线

`mini` 这个程序用一个命令行把整条编译流程串了起来，`--tokens/--ast/--ir/--run` 对应各大阶段：

```
源码 .mini
  │  lexer       词法分析：字符流 → 记号流        --tokens
  ▼
记号流
  │  parser      语法分析：记号流 → 抽象语法树       --ast
  ▼
AST
  │  ir          中间代码生成：AST → 三地址码       --ir
  ▼
三地址中间代码
  │  interpreter 解释执行：语义检查 + 虚拟机执行      --run
  ▼
输出结果
```

```bash
./build/mini.exe miniLang/examples/fib.mini --tokens --ast --ir --run
```

这会依次打印记号流、AST、三地址码、最后是程序运行结果——**一份源码走完整个编译器**。

---

## 1. 语言设计（目标语言的确定）

MiniLang 是一门**小而完整**的过程式语言，刻意只保留最能展示编译原理的东西：

| 维度 | MiniLang 的选择 |
|------|----------------|
| 值类型 | `int / float / string / bool`（标签联合，动态类型语义）|
| 语句 | `let` 声明、赋值、`return`、`print`、`if/else`、`while`、块 `{}` |
| 函数 | `func 名字(参数) { 语句 }`，参数按值传递，有返回值 |
| 注释 | `// 行注释` |
| 文法特性 | 算术/比较/逻辑运算符，运算符优先级与结合性 |

设计语言本身就是编译的第一步：**文法的形状直接决定了后面词法、语法分析器的工作量**。

示例 `examples/fib.mini`：
```
func fib(n) {
    if (n <= 1) { return n; }
    return fib(n - 1) + fib(n - 2);
}
print(fib(10));   // 55
```

---

## 2. 词法分析：字符流 → 记号流

### 要解决的问题
对编译而言，字符本身“没有意义”。`count` 和 `count1` 是不同的标识符，`+` 是运算符，`"hello"` 是一个整体字符串。**词法分析的任务，就是把一串字符切分成一系列“记号”（Token），每个记好它是什么类型、原文是什么、在第几行。**

### 核心概念
- **词法单元 / 终结符（Token / Terminal）**：源码里不可再分的最小单元，如关键字、标识符、字面量、运算符、分隔符。
- **正则表达式**：形式化地描述“什么样的字符序列是一个记号”（如 `[a-zA-Z_][a-zA-Z0-9_]*` 描述标识符）。
- **有限自动机（NFA/DFA）**：把正则表达式变成状态机来匹配字符流。手写词法分析器本质就是把 DFA 的状态转换**逐个手写成 switch**。
- **最长匹配**：读到的字符尽可能多地形成一个记号。例如读到 `=` 后还要看下一个是否也是 `=`，从而得到 `==` 而非 `=`。
- **关键字 vs 标识符**：先按标识符规则读完整串，再查关键字表判断它是保留字还是普通变量名。

### 对应代码 `lexer.cpp`
`TokenType` 枚举（`token.h`）定义了所有词法单元种类。`Lexer::tokenize()` 是扫描主循环，`scanIdentifier / scanNumber / scanString` 分别识别各类记号：

```cpp
TokenType keywordType(const std::string& text) {          // 关键字表查询
    static const std::unordered_map<std::string, TokenType> kw = {
        {"func", FUNC}, {"let", LET}, {"if", IF}, {"else", ELSE},
        {"while", WHILE}, {"print", PRINT}, {"return", RETURN},
        {"true", TRUE}, {"false", FALSE},
    };
    auto it = kw.find(text);
    return it == kw.end() ? IDENT : it->second;            // 不是关键字 → 标识符
}
```

运算符的**最长匹配**就是"读到单字符后奔着二字符扩展"：
```cpp
case '=':  type = (match('=')) ? EQ : ASSIGN;  break;   // '=' 或 '=='
case '!':  type = (match('=')) ? NEQ : NOT;    break;   // '!' 或 '!='（注意：单独的 ! 是逻辑非）
```

每个 Token 还记录 `line`，为后面的错误报告提供"第几行出错了"。

运行 `--tokens` 查看记号流（代码按"类型 + 原文 + 行号"打印）：
```
行4    LET       let
行4    IDENT     count
行4    =         =
行4    INT       0
行4    ;         ;
```

---

## 3. 语法分析：记号流 → 抽象语法树

### 要解决的问题
记号流是**扁平的线性列表**，但程序是有**层次结构**的：`a + b * c` 是"乘"套在"加"里面，`if` 后面跟着整个语句块。语法分析要把线性序列还原成树状结构。

### 核心概念
- **上下文无关文法（CFG，Context-Free Grammar）**：用产生式规则描述"一个语法结构可以由什么构成"。如 `expr → expr + term` 表示"一个表达式可以是一个表达式加一个项"。
- **自顶向下 / 自底向上**：从开始符号出发往下推导（自顶向下），或把记号逐步归约到开始符号（自底向上）。
- **递归下降（Recursive Descent）**：一种**自顶向下**实现——给每个非终结符写一个函数，一个函数里调用别的函数，函数调用关系即文法的推导关系。最直观，是学习首选。
- **优先级与结合性**：通过"把运算符分层"来保证 `*` 比 `+` 结合得更紧。低优先级的运算符先拆，高优先级后拆。
- **消除左递归 / 提取左因子**：文法若写成 `E → E + T`（左递归），递归下降会无限递归。理论上是改写文法消除左递归；MiniLang 通过**层级化写法**天然避免（见下），这也是递归下降处理表达式的标准做法。

### 对应代码 `parser.cpp`
MiniLang 的表达式按优先级分成若干层，层与层之间用函数嵌套体现"谁优先"：

```
expr ← or ← and ← equality ← relational ← additive ← multiplicative ← unary ← primary
       (低优先级) --------------------------------------------------> (高优先级)
```

```cpp
std::unique_ptr<Expr> Parser::parseAdditive() {          // 加法层
    auto left = parseMultiplicative();                    // 先"降级"解析乘法
    while (true) {
        BinOp op;
        if (match(PLUS)) op = BinOp::Add;
        else if (match(MINUS)) op = BinOp::Sub;
        else break;
        left = make_Binary(op, move(left), parseMultiplicative());
    }
    return left;
}
```

`a + b * c` 这样解析：`parseAdditive` 先调 `parseMultiplicative` 把 `b * c` 吃掉，再回来拼 `a +`，结果就是 `a + (b * c)`。**这就是"优先级通过层级化表达"直接解决了 `a + b * c` 的经典问题**，也避免了左递归死循环——不需要手工消除左递归。

运行 `--ast` 查看抽象语法树（前几行）：
```
func fib(n)
  block
    if
      bin <=
        var n
        1
    then:
      block
        return
          var n
    return
      bin +
        call fib(1 args)
          bin -
            var n
            1
        call fib(1 args)
          bin -
            var n
            2
```

---

## 4. 抽象语法树（AST）

### 要解决的问题
"语法分析树"（parse tree / concrete syntax tree）会完整保留括号、分号、每一步推导的节点——**太啰嗦**。`(a + b) * c` 的括号、语句末尾的 `;`，对后续阶段都无关紧要。

抽象语法树（Abstract Syntax Tree）**去掉这些不影响语义的细节**，只保留结构：一个表达式就是一棵"运算符 + 两个操作数"的子树。

### 对应代码 `ast.h`
用 C++ 类继承表达"节点种类"：`Expr`（表达式）与 `Stmt`（语句）两类基类，各自派生出整数、变量、二元运算、函数调用、`while`、`return` 等节点。例如二元表达式：

```cpp
struct BinaryExpr : Expr {
    BinOp op;                       // + - * / ...
    std::unique_ptr<Expr> left, right;
};
```

AST 是**前端的成品、中端的原料**：后续的中间代码生成就是遍历这棵树。

---

## 5. 中间代码生成：AST → 三地址码

### 要解决的问题
AST 还是"树的形态"，离机器很远。中间代码把它线性化成**一条条与具体机器无关的指令序列**。它是编译器的枢纽：任何高级语言都能翻译成它，它也便于做各种优化，最后再统一生成目标机代码。

### 核心概念
- **三地址码（Three-Address Code, TAC）**：每条指令右侧**至多一个运算符、至多两个操作数**，结果写入一个独立的"地址"（临时变量 `t0, t1, ...`）。
  ```
  a * b + c      →    t0 = a * b
                       t1 = t0 + c
  ```
- **临时变量（temporaries）**：存储中间结果的命名空间，是"三地址"中的第三个"地址"。
- **控制流用标签 + 跳转表达**：`if/while`、函数调用/返回，都用 `Label`、`Jump`、返回指令表示，和真实机器一致。
- **语法制导翻译（syntax-directed translation）**：遍历 AST、一边走一边发指令。每个表达式生成代码后返回一个"结果操作数"，上层把它当成操作数继续拼——**AST 的后序遍历顺序就是中间代码生成顺序**。
- **常量折叠（constant folding）**：中端最基础的优化。左右都是字面量就在编译期算掉，不生成指令：
  ```
  print(7 % 3);      →    t16 = 7; t17 = 3; t18 = t16 % t17; print t18
                        （MiniLang 对整数字面量运算做了折叠）
  ```

### 对应代码 `ir.cpp` + 真实输出
用 `examples/fib.mini --ir` 看 `fib` 的三地址码（递归函数调用也变成了"压栈"式的指令）：

```
func fib(n):
    0 t6 = n                     // 读参数 n
    1 t7 = 1
    2 t8 = t6 <= t7              // n <= 1
    3 if !t8 goto L7             // 假则跳过"直接返回 n"
    4 t9 = n
    5 return t9
    6 goto L8
    7 L7:                        // else 分支：
    9 t11 = n                    //   fib(n-1)
   11 t13 = t11 - t12            //   t13 = n - 1
   12 t10 = call fib(t13)
   13 t15 = n
   17 t17 = t15 - t16            //   t17 = n - 2
   16 t14 = call fib(t17)
   17 t18 = t10 + t14            //   两次递归结果相加
   18 return t18
```

这就是**真正的"中端"产物**——它已经和源语言的语法无关，变成了一条条可被机器执行模型解释的指令。

---

## 6. 语义与解释执行：符号表 / 作用域 / 栈帧

### 要解决的问题
三地址码是"能跑"的，但我们还要定义它**在运行时如何被理解**：
- 变量存在哪？作用域怎么划分？（**符号表 + 作用域**）
- 函数调用时参数、局部变量、返回地址怎么管理？（**栈帧**）
- `+` 遇到字符串是什么意思？`true && false` 怎么算？（**类型语义**）

### 核心概念（对应 `interpreter.cpp`）
- **值（Value）**：运行时用"标签联合"表示——一个值既可以是整数也可以是字符串，靠一个类型标签区分。算术、拼接、比较都按操作数的**运行时类型**决定行为（动态类型语义）。
- **符号表 / 环境（symbol table / environment）**：每个函数一个 `map<string, Value>`，变量名到值。
- **栈帧（stack frame / call frame）**：每次函数调用压入一个帧，帧里存：当前函数、指令指针 `ip`、本地环境、返回要写给的临时变量和恢复位置。函数返回时弹帧，把结果写回调用方。**这正是"运行时作用域"的实现——不同帧里的同名变量互不干扰。**
- **声明 vs 赋值**：`let` 用 `Decl` 指令**建立**变量；赋值用 `StoreVar` 指令、要求变量**已存在**，否则报"赋值给未声明的变量"。
- **解释执行中间代码**：一个 `while` 主循环不断取栈顶帧的当前指令、执行、推进 `ip`。它直接跑的是**上一节生成的三地址码**——这正说明了为什么中间代码层真实存在（类似字节码虚拟机）。

```cpp
int Interpreter::run() {
    // ... 初始化 main 帧压栈
    while (!stack_.empty()) {
        Frame& f = stack_.back();
        if (f.ip >= (int)f.fn->code.size()) { /* 隐式 return 0 */ }
        const Instr& in = f.fn->code[f.ip];
        switch (in.kind) {
            case Instr::K::Binary: f.env[in.dst] = doBinary(in.op, evalOperand(in.a,f), evalOperand(in.b,f)); f.ip++; break;
            case Instr::K::Call:   /* 压入被调函数的新帧 */ break;
            case Instr::K::Ret:    /* 弹帧，把结果写回调用方 */ break;
            case Instr::K::JumpIfFalse: if (!evalOperand(in.a,f).truthy()) f.ip = in.target; else f.ip++; break;
            // ...
        }
    }
}
```

运行 `fib.mini --run`，就是让这个解释器把上面的三地址码**真的执行**，最终打印出：

```
55
610
6765
```

---

## 7. 一次递归调用，串起所有阶段

以最顶层的 `print(fib(10))` 为例，看编译器与运行时分别为它做了什么：

1. **词法**：`fib` 被切成 `IDENT`，`print` 被切成 `PRINT`，`10` 被切成 `INT`，括号/分号各成一个记号。
2. **语法**：递归下降把这些记号拼成 `CallExpr(callee="fib", args=[IntLit(10)])` 套在 `PrintStmt` 里。
3. **中间代码**：生成 `t1 = 10; t0 = call fib(t1); print t0`。
4. **解释执行**：执行 `call` 时，`call()` 把实参 `10` 绑定到 `fib` 的参数 `n`，压入一个新栈帧，指令指针跳到 `fib` 的第 0 条指令。`fib` 里又出现两个 `call`，于是**再压入两帧**——帧一级一级叠起来，就是递归的"栈"。最深的帧返回后依次弹帧、把结果相加，最后回到 `main` 的 `print` 指令输出。**每一层栈帧 = 一次函数调用 = 一个独立的作用域。**

---

## 8. 错误处理（贯穿各阶段）

三个阶段的错误都以**带行号**的方式报告，这正是前面 Token 记录行号的用处：

| 阶段 | 示例 | 报错 |
|------|------|------|
| 词法 | `print(@);`（非法字符）| `词法错误: 行1: @` |
| 语法 | `print(1)`（缺分号）| `语法错误 [行 2]: 期望 ';'` |
| 运行时 | `print(1/0);`（除零）| `运行时错误：除数不能为 0` |
| 运行时 | `x = 5;`（未声明赋值）| `运行时错误：赋值给未声明的变量 'x'` |

---

## 9. 项目结构对照表

MiniLang 里**每个文件恰好对应一个编译原理概念**：

```
miniLang/
├── token.h             词法单元 / 终结符
├── lexer.h/.cpp        词法分析（手写扫描器）
├── ast.h               抽象语法树
├── parser.h/.cpp       语法分析（递归下降）
├── ir.h/.cpp           中间代码生成（三地址码 + 常量折叠优化）
├── interpreter.h/.cpp  语义 + 解释执行（符号表 / 作用域 / 栈帧）
├── main.cpp            CLI：--tokens/--ast/--ir/--run
└── examples/           示例程序
```

---

## 10. 扩展方向（让语言长大）

理解了这条骨架后，继续做下去时每一步仍是"复用上面的思想"：

- **真正的前端类型检查**：给 `let` 加类型标注（`let x: int`），在符号表里记类型，做静态类型检查（严于当前的动态类型语义）。
- **作用域下沉到块级**：目前是函数级作用域；若要块级作用域，只需让 `while/if` 的块生成独立的子环境（栈帧的概念可以延伸）。
- **更复杂的优化**：公共子表达式消除、死代码消除、循环不变式外提——都是在三地址码上做等价变换。
- **后端：生成汇编**：把三地址码映射到真实指令、用图着色做寄存器分配——这是"后端"的天地，MiniLang 暂且聚焦前端+中端。

---

> 一句话总结 MiniLang：**字符流被词法切成记号，记号被语法还原成树，树被翻译成三地址指令，指令被虚拟机一帧一帧执行** —— 这就是一台（微型）编译器从头到尾做的事。