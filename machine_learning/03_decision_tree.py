"""
03. 决策树：用“如果...那么...”规则做预测
=========================================
决策树把特征空间不断按阈值切分，每个叶子给出一个类别。
例如：“x1 < 0.1 且 x2 < 0.2 → 蓝色”。

核心问题：每次切分选哪个特征、哪个阈值？
答案是让切分后的子节点“最纯”——常用基尼不纯度
衡量混乱程度，选让纯度提升最大的切分点。

学习要点：
  - max_depth 控制树的复杂度：太浅欠拟合，太深过拟合
  - 决策树最大的优点是“可解释”：规则能直接画出来看
  - 决策树对数据小扰动敏感、容易过拟合 → 引出 04 随机森林

运行：python 03_decision_tree.py
"""

import os

import matplotlib

# 无界面后端：不弹窗，直接保存图片到 output/
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from sklearn.datasets import make_moons
from sklearn.model_selection import train_test_split
from sklearn.tree import DecisionTreeClassifier, plot_tree

plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei"]
plt.rcParams["axes.unicode_minus"] = False

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
os.makedirs(OUT, exist_ok=True)


def plot_boundary(model, X, y, ax, title):
    """画出分类模型的决策边界"""
    x_min, x_max = X[:, 0].min() - 0.3, X[:, 0].max() + 0.3
    y_min, y_max = X[:, 1].min() - 0.3, X[:, 1].max() + 0.3
    xx, yy = np.meshgrid(
        np.linspace(x_min, x_max, 300),
        np.linspace(y_min, y_max, 300),
    )
    Z = model.predict(np.c_[xx.ravel(), yy.ravel()])
    Z = Z.reshape(xx.shape)
    ax.contourf(xx, yy, Z, cmap="bwr", alpha=0.4)
    ax.contour(xx, yy, Z, levels=[0.5], colors="k", linewidths=1.5)
    ax.scatter(X[:, 0], X[:, 1], c=y, cmap="bwr", edgecolors="k", s=25)
    ax.set_title(title)
    ax.set_xlabel("特征 1")
    ax.set_ylabel("特征 2")


def main():
    # 月牙数据：两类弯月，是标准的非线性二分类问题
    X, y = make_moons(n_samples=300, noise=0.2, random_state=42)
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42
    )

    # max_depth=3：允许树长 3 层。深度越大越能“钻牛角尖”
    tree = DecisionTreeClassifier(max_depth=3, random_state=42)
    tree.fit(X_train, y_train)

    print(f"训练准确率 = {tree.score(X_train, y_train):.3f}")
    print(f"测试准确率 = {tree.score(X_test, y_test):.3f}")
    print(f"树的节点数 = {tree.tree_.node_count}，最大深度 = {tree.tree_.max_depth}")

    fig, axes = plt.subplots(1, 2, figsize=(16, 6))

    plot_boundary(tree, X_train, y_train, axes[0], "深度=3 的决策边界")

    # plot_tree 直接把 if-else 规则画出来（左图数据对应的树）
    plot_tree(
        tree,
        ax=axes[1],
        feature_names=["x1", "x2"],
        class_names=["类别0", "类别1"],
        filled=True,
        rounded=True,
        fontsize=9,
    )
    axes[1].set_title("决策树结构：每个节点是一条 if-else 规则")

    plt.tight_layout()
    out_path = os.path.join(OUT, "03_decision_tree.png")
    plt.savefig(out_path, dpi=120, bbox_inches="tight")
    print("决策边界 + 树结构图已保存到:", out_path)


if __name__ == "__main__":
    main()
