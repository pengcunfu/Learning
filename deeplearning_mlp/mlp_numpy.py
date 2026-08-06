"""
MLP（多层感知机）从零实现 —— 纯 NumPy 版本
===========================================

本文件用 NumPy 手写一个最简的多层感知机，目标是让你看懂每一个数学步骤：
    1. 前向传播（Forward）
    2. 反向传播（Backpropagation / 链式法则求梯度）
    3. 梯度下降更新参数

数据集使用 sklearn 的 "make_moons"（月牙形二分类数据）。
它是一个典型的“线性不可分”问题 —— 逻辑回归画一条直线无法分开，
但 MLP 加上一层隐藏层 + 非线性激活就能轻松搞定。

运行：python mlp_numpy.py
依赖：pip install numpy matplotlib scikit-learn
"""

import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import make_moons
from sklearn.model_selection import train_test_split

# 让 matplotlib 能正常显示中文标题（Windows 自带黑体 / 微软雅黑）
plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei"]
plt.rcParams["axes.unicode_minus"] = False  # 正常显示负号


# ----------------------------------------------------------------------
# 激活函数与工具
# ----------------------------------------------------------------------
def relu(z):
    """ReLU：max(0, z)，隐藏层最常用的激活函数"""
    return np.maximum(0, z)


def relu_backward(dA, z):
    """ReLU 的导数：z > 0 时为 1，否则为 0"""
    dz = np.array(dA, copy=True)
    dz[z <= 0] = 0
    return dz


def sigmoid(z):
    """Sigmoid：把输出压到 (0,1) 区间，适合二分类输出层"""
    return 1 / (1 + np.exp(-np.clip(z, -500, 500)))  # clip 防止数值溢出


class MLP:
    """
    一个两层（单隐藏层）的多层感知机：
        输入层 -> 隐藏层(ReLU) -> 输出层(Sigmoid)

    结构对应公式：
        Z1 = X @ W1 + b1
        A1 = ReLU(Z1)
        Z2 = A1 @ W2 + b2
        A2 = Sigmoid(Z2)        # 预测概率
    """

    def __init__(self, input_dim, hidden_dim, output_dim, seed=42):
        rng = np.random.default_rng(seed)
        # He 初始化（专为 ReLU 设计）：方差 = 2/输入维度，避免梯度消失或爆炸。
        # 直接用 *0.01 这种很小的初始化，配合 ReLU 会让网络几乎学不动。
        self.W1 = rng.standard_normal((input_dim, hidden_dim)) * np.sqrt(2.0 / input_dim)
        self.b1 = np.zeros((1, hidden_dim))
        self.W2 = rng.standard_normal((hidden_dim, output_dim)) * np.sqrt(2.0 / hidden_dim)
        self.b2 = np.zeros((1, output_dim))

    # ------------------------------------------------------------------
    # 前向传播：把输入一路算到预测概率
    # ------------------------------------------------------------------
    def forward(self, X):
        self.Z1 = X @ self.W1 + self.b1
        self.A1 = relu(self.Z1)
        self.Z2 = self.A1 @ self.W2 + self.b2
        self.A2 = sigmoid(self.Z2)
        return self.A2

    # ------------------------------------------------------------------
    # 反向传播：用链式法则从输出往回算每个参数的梯度
    # ------------------------------------------------------------------
    def backward(self, X, Y):
        m = X.shape[0]  # 样本数，用于对 batch 取平均

        # 输出层梯度（使用交叉熵 + sigmoid 时，dA2 的形式恰好如此）
        dZ2 = self.A2 - Y                       # (m, output_dim)
        self.dW2 = self.A1.T @ dZ2 / m          # (hidden_dim, output_dim)
        self.db2 = np.sum(dZ2, axis=0, keepdims=True) / m

        # 传回隐藏层
        dA1 = dZ2 @ self.W2.T
        dZ1 = relu_backward(dA1, self.Z1)
        self.dW1 = X.T @ dZ1 / m                # (input_dim, hidden_dim)
        self.db1 = np.sum(dZ1, axis=0, keepdims=True) / m

    # ------------------------------------------------------------------
    # 参数更新：梯度下降
    # ------------------------------------------------------------------
    def step(self, lr):
        self.W1 -= lr * self.dW1
        self.b1 -= lr * self.db1
        self.W2 -= lr * self.dW2
        self.b2 -= lr * self.db2

    @staticmethod
    def loss(A2, Y):
        """二元交叉熵损失（BCE）"""
        eps = 1e-8
        return -np.mean(Y * np.log(A2 + eps) + (1 - Y) * np.log(1 - A2 + eps))


# ----------------------------------------------------------------------
# 准备数据
# ----------------------------------------------------------------------
X, y = make_moons(n_samples=500, noise=0.2, random_state=42)
y = y.reshape(-1, 1)  # 变成列向量 (m, 1)

X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)

# 简单的标准化（让特征均值为 0、方差为 1），有助于训练稳定
mean, std = X_train.mean(0), X_train.std(0)
X_train = (X_train - mean) / std
X_test = (X_test - mean) / std

# ----------------------------------------------------------------------
# 训练
# ----------------------------------------------------------------------
model = MLP(input_dim=2, hidden_dim=8, output_dim=1)
learning_rate = 0.5
epochs = 1000

history = []
for epoch in range(epochs):
    probs = model.forward(X_train)            # 前向
    loss = model.loss(probs, y_train)         # 算损失
    model.backward(X_train, y_train)          # 反向
    model.step(learning_rate)                 # 更新

    history.append(loss)
    if epoch % 100 == 0:
        acc = ((probs >= 0.5) == y_train).mean()
        print(f"epoch {epoch:4d}  loss={loss:.4f}  train_acc={acc:.4f}")

# ----------------------------------------------------------------------
# 评估
# ----------------------------------------------------------------------
test_probs = model.forward(X_test)
test_acc = ((test_probs >= 0.5) == y_test).mean()
print(f"\n测试集准确率: {test_acc:.4f}")

# ----------------------------------------------------------------------
# 可视化：决策边界 + 损失曲线
# ----------------------------------------------------------------------
fig, axes = plt.subplots(1, 2, figsize=(12, 5))

# 左图：决策边界
ax = axes[0]
x_min, y_min = X_train.min(0)
x_max, y_max = X_train.max(0)
xx, yy = np.meshgrid(np.linspace(x_min - 0.5, x_max + 0.5, 200),
                     np.linspace(y_min - 0.5, y_max + 0.5, 200))
grid = np.c_[xx.ravel(), yy.ravel()]
zz = model.forward(grid).reshape(xx.shape)
ax.contourf(xx, yy, zz, levels=20, cmap="RdBu", alpha=0.6)
ax.contour(xx, yy, zz, levels=[0.5], colors="black", linewidths=1.5)
ax.scatter(X_test[:, 0], X_test[:, 1], c=y_test.ravel(), cmap="RdBu",
           edgecolors="k", s=40)
ax.set_title("MLP 决策边界（黑色线 = 分类边界）")

# 右图：损失曲线
axes[1].plot(history)
axes[1].set_xlabel("epoch")
axes[1].set_ylabel("loss (BCE)")
axes[1].set_title("训练损失曲线")

plt.tight_layout()
plt.savefig("mlp_numpy_result.png", dpi=120)
print("结果已保存到 mlp_numpy_result.png")
