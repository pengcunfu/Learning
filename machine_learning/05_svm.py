"""
05. SVM 支持向量机：找一条“最宽”的分界线
==========================================
SVM 不只找一条能分开两类的线，而是找一条离所有点
都尽量远的线——间隔越大，泛化越稳。

关键问题：月牙数据用直线根本分不开怎么办？
核函数（kernel）把数据映射到更高维空间，
数据在高维里变得线性可分，再找最大间隔超平面。

对比两个核：
  - linear 核：只能画直线 → 对月牙数据无能为力
  - RBF 核：能画任意弯曲的边界 → 效果明显更好

运行：python 05_svm.py
"""

import os

import matplotlib

# 无界面后端：不弹窗，直接保存图片到 output/
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from sklearn.datasets import make_moons
from sklearn.model_selection import train_test_split
from sklearn.svm import SVC

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

    # 支持向量：决定边界的少数关键点，用空心圆标出来
    if hasattr(model, "support_vectors_"):
        sv = model.support_vectors_
        ax.scatter(
            sv[:, 0], sv[:, 1], s=90,
            facecolors="none", edgecolors="k", linewidths=1.5,
            label="支持向量",
        )
        ax.legend(loc="best")

    ax.set_title(title)
    ax.set_xlabel("特征 1")
    ax.set_ylabel("特征 2")


def main():
    X, y = make_moons(n_samples=200, noise=0.15, random_state=42)
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42
    )

    linear_svm = SVC(kernel="linear", random_state=42)   # 只能画直线
    rbf_svm = SVC(kernel="rbf", random_state=42)         # 高斯核，可弯曲
    linear_svm.fit(X_train, y_train)
    rbf_svm.fit(X_train, y_train)

    print(f"线性核  测试准确率 = {linear_svm.score(X_test, y_test):.3f}")
    print(f"RBF 核  测试准确率 = {rbf_svm.score(X_test, y_test):.3f}")
    print(f"RBF 核的支持向量数 = {len(rbf_svm.support_vectors_)}（共 {len(X_train)} 个训练样本）")

    fig, axes = plt.subplots(1, 2, figsize=(13, 5.5))
    plot_boundary(linear_svm, X_train, y_train, axes[0], "线性核：只能画直线")
    plot_boundary(rbf_svm, X_train, y_train, axes[1], "RBF 核：可弯曲的边界")

    plt.tight_layout()
    out_path = os.path.join(OUT, "05_svm.png")
    plt.savefig(out_path, dpi=120, bbox_inches="tight")
    print("两种核的决策边界图已保存到:", out_path)


if __name__ == "__main__":
    main()
