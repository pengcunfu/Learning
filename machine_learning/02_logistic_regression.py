"""
02. 逻辑回归：线性模型做“分类”
===============================
名字里有“回归”，但逻辑回归解决的是分类问题。
它输出一个 0~1 之间的概率，比如“垃圾邮件的概率 = 0.92”。

工作原理只有两步：
    1. 线性得分：z = w1*x1 + w2*x2 + b
    2. Sigmoid 压缩：p = 1 / (1 + e^(-z))，把任意 z 压到 (0, 1)
然后以 0.5 为阈值：p >= 0.5 判为类别 1，否则判为类别 0。

学习要点：
  - 决策边界是一条直线（线性模型）
  - 输出是概率而非硬标签，能表达“置信度”
  - 评估不只看准确率，还要看混淆矩阵（哪些类别容易搞混）

运行：python 02_logistic_regression.py
"""

import os

import matplotlib

# 无界面后端：不弹窗，直接保存图片到 output/
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from sklearn.datasets import make_classification
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score, confusion_matrix
from sklearn.model_selection import train_test_split

plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei"]
plt.rcParams["axes.unicode_minus"] = False

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
os.makedirs(OUT, exist_ok=True)


def plot_decision_boundary(model, X, y, ax, title):
    """画出模型的概率热力图 + 0.5 决策边界 + 原始样本点"""
    x_min, x_max = X[:, 0].min() - 0.5, X[:, 0].max() + 0.5
    y_min, y_max = X[:, 1].min() - 0.5, X[:, 1].max() + 0.5
    xx, yy = np.meshgrid(
        np.linspace(x_min, x_max, 200),
        np.linspace(y_min, y_max, 200),
    )

    # predict_proba 返回 [P(类别0), P(类别1)]，取类别1的概率
    Z = model.predict_proba(np.c_[xx.ravel(), yy.ravel()])[:, 1]
    Z = Z.reshape(xx.shape)

    # 背景颜色 = 概率；黑色等高线 0.5 就是决策边界
    cf = ax.contourf(xx, yy, Z, levels=50, cmap="bwr", alpha=0.7)
    ax.contour(xx, yy, Z, levels=[0.5], colors="k", linewidths=2)
    ax.scatter(X[:, 0], X[:, 1], c=y, cmap="bwr", edgecolors="k", s=25)
    ax.set_title(title)
    ax.set_xlabel("特征 1")
    ax.set_ylabel("特征 2")
    return cf


def main():
    # 两类、两特征、基本线性可分的数据（flip_y 加 5% 标签噪声，更真实）
    X, y = make_classification(
        n_samples=200,
        n_features=2,
        n_informative=2,
        n_redundant=0,
        n_clusters_per_class=1,
        flip_y=0.05,
        random_state=42,
    )
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42
    )

    model = LogisticRegression()
    model.fit(X_train, y_train)

    print(f"学到的权重 w = {model.coef_[0].round(3)}，截距 b = {model.intercept_[0]:.3f}")
    print(f"测试准确率 = {accuracy_score(y_test, model.predict(X_test)):.3f}")

    # 混淆矩阵：行 = 真实标签，列 = 预测标签
    cm = confusion_matrix(y_test, model.predict(X_test))
    print("混淆矩阵（行=真实，列=预测）：")
    print(cm)
    print("  左上 = 真实0预测0（正确）   右上 = 真实0预测1（误报）")
    print("  左下 = 真实1预测0（漏报）   右下 = 真实1预测1（正确）")

    # 挑一个样本看概率输出（而不是硬标签）
    proba = model.predict_proba(X_test[:1])[0]
    print(f"\n示例样本: P(类别0) = {proba[0]:.3f}, P(类别1) = {proba[1]:.3f}")

    fig, ax = plt.subplots(figsize=(8, 6))
    cf = plot_decision_boundary(
        model, X_train, y_train, ax,
        "逻辑回归：颜色=概率，黑线=决策边界(0.5)",
    )
    fig.colorbar(cf, ax=ax, label="P(类别 1)")

    plt.tight_layout()
    out_path = os.path.join(OUT, "02_logistic_regression.png")
    plt.savefig(out_path, dpi=120, bbox_inches="tight")
    print("决策边界图已保存到:", out_path)


if __name__ == "__main__":
    main()
