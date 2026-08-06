"""
08. 模型评估：怎么看模型会不会“翻车”
=======================================
最常见的坑：训练集上 95 分，一到新数据只有 70 分
——这就是过拟合（背题背多了，没学会解题）。

本示例把“树的深度”当作复杂度旋钮：
  - 深度小 → 欠拟合（太简单，两边都差）
  - 深度合适 → 泛化最好
  - 深度大 → 过拟合（训练满分，测试下滑）
训练/测试两条曲线分开得越宽，过拟合越严重。

再介绍 K 折交叉验证：数据切成 K 份，轮流拿 1 份
当验证、K-1 份训练，最终取平均——比一次随机切分
更稳，能减少“运气成分”。

运行：python 08_model_evaluation.py
"""

import os

import matplotlib

# 无界面后端：不弹窗，直接保存图片到 output/
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from sklearn.datasets import make_classification
from sklearn.model_selection import cross_val_score, train_test_split
from sklearn.tree import DecisionTreeClassifier

plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei"]
plt.rcParams["axes.unicode_minus"] = False

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
os.makedirs(OUT, exist_ok=True)


def main():
    # flip_y=0.1 表示 10% 的标签是噪声 → 数据本身“学不干净”，过拟合更容易暴露
    X, y = make_classification(
        n_samples=500,
        n_features=2,
        n_informative=2,
        n_redundant=0,
        n_clusters_per_class=1,
        flip_y=0.1,
        random_state=42,
    )
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.3, random_state=42
    )

    # ---------- 1) 不同深度下的过拟合曲线 ----------
    depths = range(1, 16)
    train_scores, test_scores = [], []
    for d in depths:
        model = DecisionTreeClassifier(max_depth=d, random_state=42)
        model.fit(X_train, y_train)
        train_scores.append(model.score(X_train, y_train))
        test_scores.append(model.score(X_test, y_test))

    print("深度  训练准确率  测试准确率")
    for d, tr, te in zip(depths, train_scores, test_scores):
        flag = " <- 测试开始下滑（过拟合）" if d >= 8 and tr - te > 0.15 else ""
        print(f"{d:>3}   {tr:.3f}       {te:.3f}{flag}")

    fig, axes = plt.subplots(1, 2, figsize=(14, 5.5))

    axes[0].plot(depths, train_scores, "o-", label="训练集准确率")
    axes[0].plot(depths, test_scores, "s-", label="测试集准确率")
    axes[0].set_title("过拟合曲线：训练/测试准确率 vs 树的深度")
    axes[0].set_xlabel("max_depth（复杂度）")
    axes[0].set_ylabel("准确率")
    axes[0].legend()
    axes[0].grid(True, alpha=0.3)

    # ---------- 2) 5 折交叉验证 ----------
    model = DecisionTreeClassifier(max_depth=3, random_state=42)
    scores = cross_val_score(model, X, y, cv=5)
    print("\n5 折交叉验证准确率:", np.round(scores, 3))
    print(f"均值 = {scores.mean():.3f} ± 标准差 {scores.std():.3f}")

    axes[1].bar(range(1, 6), scores)
    axes[1].axhline(scores.mean(), color="red", linestyle="--", label=f"均值 {scores.mean():.3f}")
    axes[1].set_title("5 折交叉验证：每折的准确率")
    axes[1].set_xlabel("折数（fold）")
    axes[1].set_ylabel("准确率")
    axes[1].set_ylim(0.5, 1.0)
    axes[1].legend()
    axes[1].grid(True, alpha=0.3)

    plt.tight_layout()
    out_path = os.path.join(OUT, "08_model_evaluation.png")
    plt.savefig(out_path, dpi=120, bbox_inches="tight")
    print("评估图已保存到:", out_path)


if __name__ == "__main__":
    main()
