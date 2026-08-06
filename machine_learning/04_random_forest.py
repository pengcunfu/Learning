"""
04. 随机森林：一群决策树“投票”
================================
单棵决策树很容易过拟合：训练集几乎 100% 正确，
一到测试集就掉链子。随机森林的想法很朴素——
训练许多棵“各不相同”的决策树，让它们投票，
整体结果比任何一棵树都稳。

为什么树会各不相同？
  - 每棵树只用随机抽取的部分样本训练（Bagging）
  - 每次切分只从随机抽取的部分特征里选

学习要点：
  - 集成学习：多个弱模型合成一个强模型
  - “平均/投票”大幅降低方差，泛化更好
  - 对比单棵树，重点看训练/测试准确率的差距

运行：python 04_random_forest.py
"""

import os

import matplotlib

# 无界面后端：不弹窗，直接保存图片到 output/
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from sklearn.datasets import make_moons
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.tree import DecisionTreeClassifier

plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei"]
plt.rcParams["axes.unicode_minus"] = False

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
os.makedirs(OUT, exist_ok=True)


def plot_boundary(model, X, y, ax, title):
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
    # noise 调大一点，让“单棵树过拟合”的现象更明显
    X, y = make_moons(n_samples=400, noise=0.3, random_state=42)
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.25, random_state=42
    )

    # 单棵决策树：不限制深度，让它自由发挥（几乎必然过拟合）
    single = DecisionTreeClassifier(random_state=42)
    single.fit(X_train, y_train)

    # 随机森林：100 棵决策树投票
    forest = RandomForestClassifier(n_estimators=100, random_state=42)
    forest.fit(X_train, y_train)

    print("模型               训练准确率    测试准确率")
    print(f"单棵决策树         {single.score(X_train, y_train):.3f}        {single.score(X_test, y_test):.3f}")
    print(f"随机森林(100棵)    {forest.score(X_train, y_train):.3f}        {forest.score(X_test, y_test):.3f}")
    print("注意看：单棵树训练分很高但测试分低（过拟合），随机森林两者差距小得多")

    fig, axes = plt.subplots(1, 2, figsize=(13, 5.5))
    plot_boundary(single, X_train, y_train, axes[0], "单棵决策树（容易过拟合）")
    plot_boundary(forest, X_train, y_train, axes[1], "随机森林 100 棵树投票")

    plt.tight_layout()
    out_path = os.path.join(OUT, "04_random_forest.png")
    plt.savefig(out_path, dpi=120, bbox_inches="tight")
    print("对比图已保存到:", out_path)


if __name__ == "__main__":
    main()
