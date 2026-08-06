"""
07. PCA 主成分分析：给高维数据“降维”
======================================
手写数字图是 8x8 = 64 个像素，也就是 64 维特征。
人眼没法看 64 维，PCA 可以找出数据变化最大的
几个方向（主成分），把 64 维压缩到 2 维可视化，
同时尽量保留信息。

学习要点：
  - 降维用途：可视化、压缩、去噪、加速训练
  - 方差解释率：每个主成分携带了多少信息
  - 降维是有损的，压缩得越多丢的信息越多

运行：python 07_pca.py
（digits 是 sklearn 自带数据集，不需要联网下载）
"""

import os

import matplotlib

# 无界面后端：不弹窗，直接保存图片到 output/
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from sklearn.datasets import load_digits
from sklearn.decomposition import PCA

plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei"]
plt.rcParams["axes.unicode_minus"] = False

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
os.makedirs(OUT, exist_ok=True)


def main():
    digits = load_digits()
    X = digits.data   # shape = (1797, 64)：1797 张 8x8 图展平成 64 维
    y = digits.target  # 每张图对应的数字 0~9

    print(f"数据 shape: {X.shape}，即 {X.shape[0]} 张 8x8 手写数字，每张展开成 {X.shape[1]} 维")

    # ---------- 1) PCA 降维到 2 维 ----------
    pca = PCA(n_components=2)
    X_2d = pca.fit_transform(X)

    kept = pca.explained_variance_ratio_.sum()
    print(f"前 2 个主成分解释的方差占比 = {kept:.3f}"
          f"（其余 {1 - kept:.3f} 的信息被丢弃）")

    # ---------- 2) 画 1x3 三幅图 ----------
    fig, axes = plt.subplots(1, 3, figsize=(17, 5.2))

    # 左边：把 8 张原始数字图拼成一条横带
    montage = np.hstack([digits.images[i] for i in range(8)])
    axes[0].imshow(montage, cmap="gray")
    axes[0].set_title("原始 8x8 数字图（0~7）")
    axes[0].axis("off")

    # 中间：64 维压到 2 维后的散点，颜色 = 真实数字
    sc = axes[1].scatter(X_2d[:, 0], X_2d[:, 1], c=y, cmap="tab10", s=12, alpha=0.7)
    axes[1].set_title("PCA 降到 2 维（颜色=数字 0~9）")
    axes[1].set_xlabel("主成分 1")
    axes[1].set_ylabel("主成分 2")
    fig.colorbar(sc, ax=axes[1], ticks=range(10))

    # 右边：累计方差解释率曲线（要多少维才能保留 95% 信息？）
    pca_full = PCA().fit(X)
    cum_ratio = pca_full.explained_variance_ratio_.cumsum()
    axes[2].plot(range(1, len(cum_ratio) + 1), cum_ratio)
    axes[2].axhline(0.95, color="red", linestyle="--", label="95% 信息线")
    axes[2].set_title("累计方差解释率：主成分越多信息越多")
    axes[2].set_xlabel("主成分个数")
    axes[2].set_ylabel("累计解释率")
    axes[2].legend()
    axes[2].grid(True, alpha=0.3)

    plt.tight_layout()
    out_path = os.path.join(OUT, "07_pca.png")
    plt.savefig(out_path, dpi=120, bbox_inches="tight")
    print("PCA 可视化已保存到:", out_path)


if __name__ == "__main__":
    main()
