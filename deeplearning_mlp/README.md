# MLP（多层感知机）样例

本目录用两个版本演示同一个东西——多层感知机（MLP），从“看懂原理”到“工程实践”。

| 文件 | 实现方式 | 任务 | 目的 |
|------|----------|------|------|
| `mlp_numpy.py` | 纯 NumPy 手写 | 月牙形二分类 | 看懂前向传播、反向传播、梯度下降每一步 |
| `mlp_pytorch.py` | PyTorch | MNIST 手写数字识别 | 学会用框架实战（含 DataLoader / Adam / 训练评估循环） |

## 运行

```bash
# 安装依赖
pip install numpy matplotlib scikit-learn torch torchvision

# 跑纯 NumPy 版（秒级出图）
python mlp_numpy.py

# 跑 PyTorch 版（首次会自动下载 MNIST 数据集）
python mlp_pytorch.py
```

---

## MLP 是什么

多层感知机是最基础的**前馈神经网络**，把若干个“全连接层（Linear）”用非线性激活函数串起来：

```
输入 X  →  [隐藏层 + ReLU] × N  →  输出层  →  预测
```

**为什么要隐藏层和激活函数？**
- 只有一个输出层 = 逻辑回归/线性回归，**只能学线性关系**。
- 加上隐藏层 + 非线性激活（ReLU、Sigmoid 等），就能拟合任意复杂的非线性边界。
- `mlp_numpy.py` 里的月牙数据正好说明这点：线性模型画一条直线分不开，MLP 可以。

## 三个核心步骤

### 1. 前向传播（Forward）
逐层计算，得到预测输出。以单隐藏层为例：

```
Z1 = X @ W1 + b1      # 第一层线性变换（@ 是矩阵乘法）
A1 = ReLU(Z1)          # 非线性激活
Z2 = A1 @ W2 + b2     # 输出层
A2 = Sigmoid(Z2)       # 概率
```

### 2. 损失函数（Loss）
衡量预测和真实标签的差距：
- 二分类：二元交叉熵（BCE）—— 见 `mlp_numpy.py`
- 多分类：交叉熵（CrossEntropy）—— 见 `mlp_pytorch.py`

### 3. 反向传播 + 梯度下降（Backprop + SGD）
- **反向传播**：用链式法则从输出往回，算出损失对每个参数的梯度。
- **梯度下降**：沿着梯度反方向更新参数 `W = W - lr * dW`，让损失一点点变小。

PyTorch 把第 3 步全部自动化了（`loss.backward()` + `optimizer.step()`），
而 `mlp_numpy.py` 把这一步手写出来，方便你理解 autograd 背后在做什么。

## 常见超参数

| 参数 | 含义 | 典型值 |
|------|------|--------|
| `hidden_dim` | 隐藏层神经元数 | 64~512 |
| `lr` | 学习率（步长） | 1e-3 ~ 1e-2 |
| `epochs` | 遍历整个数据集的次数 | 5~50 |
| `batch_size` | 每次更新的样本数 | 32~256 |

学习率太大→不收敛；太小→收敛慢。可以先调它，再调网络结构。

## 下一步

- 给 MLP 加 `Dropout`（防过拟合）、`BatchNorm`（加速收敛）
- 看到 MLP 处理图像不如 CNN，处理序列不如 RNN/Transformer —— 这是后续学习的方向
