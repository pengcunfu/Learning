"""
00. 数据集总览：先看看后面要用到的四种数据
===========================================
机器学习第一步是“认识数据”。本脚本生成并画出后面示例
会用到的四种合成数据集，让你对它们的形状有直观感受：

  - 回归数据：X 一个特征，y 是连续数值 → 用“线”拟合
  - 分类数据：两类，y 是离散标签 → 用“线”分开
  - 月牙数据：两条弯月，线性模型分不开 → 考验非线性模型
  - 簇状数据：没有标签 → 聚类算法自己找“帮派”

运行：python 00_generate_datasets.py
依赖：numpy matplotlib scikit-learn
"""

import os

import matplotlib

# 无界面后端：不弹窗，直接保存图片到 output/（学习时看图片文件即可）
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from sklearn.datasets import (
    make_blobs,
    make_classification,
    make_moons,
    make_regression,
)

# 让 matplotlib 能正常显示中文标题（Windows 自带微软雅黑 / 黑体）
plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei"]
plt.rcParams["axes.unicode_minus"] = False  # 正常显示负号

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
os.makedirs(OUT, exist_ok=True)


def main():
    # 1) 回归数据：n_features=1，方便画在二维图上
    X_reg, y_reg = make_regression(n_samples=80, n_features=1, noise=15, random_state=42)

    # 2) 分类数据：2 个特征、2 个类别，两个类别基本可用一条直线分开
    X_cls, y_cls = make_classification(
        n_samples=200,
        n_features=2,
        n_informative=2,       # 两个特征都有区分信息
        n_redundant=0,         # 不要冗余特征
        n_clusters_per_class=1,
        random_state=42,
    )

    # 3) 月牙数据：经典“非线性可分”玩具数据
    X_moon, y_moon = make_moons(n_samples=300, noise=0.15, random_state=42)

    # 4) 簇状数据：四个点团（真实标签只用来上色，聚类时不会用到）
    X_blob, y_blob = make_blobs(n_samples=300, centers=4, cluster_std=1.2, random_state=42)

    fig, axes = plt.subplots(2, 2, figsize=(12, 10))

    axes[0, 0].scatter(X_reg, y_reg, s=30, alpha=0.8)
    axes[0, 0].set_title("① 回归数据：y 是连续数值")
    axes[0, 0].set_xlabel("特征 X")
    axes[0, 0].set_ylabel("目标 y")

    axes[0, 1].scatter(X_cls[:, 0], X_cls[:, 1], c=y_cls, cmap="bwr", s=30, alpha=0.8)
    axes[0, 1].set_title("② 分类数据：两类，可被直线分开")
    axes[0, 1].set_xlabel("特征 1")
    axes[0, 1].set_ylabel("特征 2")

    axes[1, 0].scatter(X_moon[:, 0], X_moon[:, 1], c=y_moon, cmap="bwr", s=30, alpha=0.8)
    axes[1, 0].set_title("③ 月牙数据：非线性可分")
    axes[1, 0].set_xlabel("特征 1")
    axes[1, 0].set_ylabel("特征 2")

    axes[1, 1].scatter(X_blob[:, 0], X_blob[:, 1], c=y_blob, cmap="viridis", s=30, alpha=0.8)
    axes[1, 1].set_title("④ 簇状数据：没有标签，聚类自己找")
    axes[1, 1].set_xlabel("特征 1")
    axes[1, 1].set_ylabel("特征 2")

    for ax in axes.flat:
        ax.grid(True, alpha=0.3)

    plt.tight_layout()
    out_path = os.path.join(OUT, "00_datasets.png")
    plt.savefig(out_path, dpi=120, bbox_inches="tight")

    print("四种数据集的 shape（样本数, 特征数）-> 标签数：")
    print("  回归数据:", X_reg.shape, "->", y_reg.shape)
    print("  分类数据:", X_cls.shape, "->", y_cls.shape)
    print("  月牙数据:", X_moon.shape, "->", y_moon.shape)
    print("  簇状数据:", X_blob.shape, "->", y_blob.shape)
    print("总览图已保存到:", out_path)


if __name__ == "__main__":
    main()
