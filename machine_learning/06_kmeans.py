"""
06. K-Means 聚类：无监督学习，自动“分帮派”
============================================
之前的例子都有标签 y（监督学习）。聚类没有标签，
只有特征 X，算法自动把相似的样本归为一组。

K-Means 的迭代思路：
    1. 随机放 K 个初始中心
    2. 每个样本归到离它最近的中心
    3. 每组样本的均值成为新中心
    4. 重复 2~3，直到中心不再变化

K 怎么选？肘部法则：
    画 K 与“总簇内距离”（inertia）的曲线，
    找拐弯最明显的位置——再增加 K 收益骤减。

运行：python 06_kmeans.py
"""

import os

import matplotlib

# 无界面后端：不弹窗，直接保存图片到 output/
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
from sklearn.datasets import make_blobs

plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei"]
plt.rcParams["axes.unicode_minus"] = False

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
os.makedirs(OUT, exist_ok=True)


def main():
    # 4 个“帮派”，cluster_std 控制每个帮派有多散
    X, y_true = make_blobs(n_samples=300, centers=4, cluster_std=1.2, random_state=42)

    # 注意：这里假装不知道有 4 个类，让算法自己分
    kmeans = KMeans(n_clusters=4, n_init=10, random_state=42)
    y_pred = kmeans.fit_predict(X)

    print("算法找到的 4 个中心点：")
    print(kmeans.cluster_centers_.round(2))
    print("总簇内距离 (inertia) =", round(kmeans.inertia_, 1))

    fig, axes = plt.subplots(1, 2, figsize=(14, 5.5))

    # ---------- 图1: 聚类结果 ----------
    axes[0].scatter(X[:, 0], X[:, 1], c=y_pred, cmap="viridis", s=30, alpha=0.8)
    axes[0].scatter(
        kmeans.cluster_centers_[:, 0], kmeans.cluster_centers_[:, 1],
        c="red", marker="X", s=200, label="聚类中心",
    )
    axes[0].set_title("K-Means 聚类结果（K=4）")
    axes[0].set_xlabel("特征 1")
    axes[0].set_ylabel("特征 2")
    axes[0].legend()

    # ---------- 图2: 肘部法则选 K ----------
    k_range = range(1, 11)
    inertias = [
        KMeans(n_clusters=k, n_init=10, random_state=42).fit(X).inertia_
        for k in k_range
    ]

    axes[1].plot(k_range, inertias, "o-")
    axes[1].axvline(4, color="red", linestyle="--", label="真实簇数=4（拐点）")
    axes[1].set_title("肘部法则：K 越大总距离越小，拐点后收益骤减")
    axes[1].set_xlabel("K（簇数）")
    axes[1].set_ylabel("总簇内距离 inertia")
    axes[1].legend()

    for ax in axes:
        ax.grid(True, alpha=0.3)

    plt.tight_layout()
    out_path = os.path.join(OUT, "06_kmeans.png")
    plt.savefig(out_path, dpi=120, bbox_inches="tight")
    print("聚类结果 + 肘部法则图已保存到:", out_path)


if __name__ == "__main__":
    main()
