"""
01. 线性回归：预测一个连续数值
===============================
场景：已知房子的面积 X，预测房价 y。
模型假设两者近似成直线关系：y = w * X + b

核心思想：找一条直线，让所有样本点到它的“竖直距离”
平均起来最小。这个距离叫损失，最常用的是均方误差 MSE：
    MSE = mean( (y_pred - y_true)^2 )

两个要点：
  - R² 越接近 1 越好，1 表示模型完美解释了数据
  - 线性回归只能画直线；但把 x 加工成 x²、x³ 等新特征后
    再套线性回归，就能拟合曲线 —— 这就是“特征工程”

运行：python 01_linear_regression.py
"""

import os

import matplotlib

# 无界面后端：不弹窗，直接保存图片到 output/
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from sklearn.datasets import make_regression
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error, r2_score
from sklearn.model_selection import train_test_split
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import PolynomialFeatures

plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei"]
plt.rcParams["axes.unicode_minus"] = False

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
os.makedirs(OUT, exist_ok=True)


def main():
    # ---------- 1) 生成数据（一个特征，方便画图） ----------
    X, y = make_regression(n_samples=100, n_features=1, noise=18, random_state=42)

    # 为什么必须切分训练/测试？如果只在训练数据上评分，
    # 模型“背题”也能拿高分，看不出真实水平
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42
    )

    # ---------- 2) 训练简单线性回归 ----------
    lr = LinearRegression()
    lr.fit(X_train, y_train)

    w = lr.coef_[0]
    b = lr.intercept_
    print(f"学到的直线: y = {w:.2f} * x + {b:.2f}")

    y_train_pred = lr.predict(X_train)
    y_test_pred = lr.predict(X_test)
    print(f"[直线]   训练 R2 = {r2_score(y_train, y_train_pred):.3f}")
    print(f"[直线]   测试 R2 = {r2_score(y_test, y_test_pred):.3f}")
    print(f"[直线]   测试 MSE = {mean_squared_error(y_test, y_test_pred):.1f}")

    # ---------- 3) 多项式回归：把 x 变成 [x, x², x³] 再套线性回归 ----------
    poly = make_pipeline(PolynomialFeatures(degree=3), LinearRegression())
    poly.fit(X_train, y_train)
    poly_test_pred = poly.predict(X_test)
    print(f"[三次曲线] 测试 R2 = {r2_score(y_test, poly_test_pred):.3f}")

    # ---------- 4) 画图对比 ----------
    x_line = np.linspace(X.min(), X.max(), 200).reshape(-1, 1)

    fig, axes = plt.subplots(1, 2, figsize=(13, 5))

    axes[0].scatter(X_train, y_train, s=25, alpha=0.7, label="训练集")
    axes[0].scatter(X_test, y_test, s=25, alpha=0.7, marker="^", label="测试集")
    axes[0].plot(x_line, lr.predict(x_line), "r-", linewidth=2, label="拟合直线")
    axes[0].set_title("线性回归（只能画直线）")
    axes[0].legend()

    axes[1].scatter(X_train, y_train, s=25, alpha=0.7, label="训练集")
    axes[1].plot(x_line, poly.predict(x_line), "g-", linewidth=2, label="三次多项式曲线")
    axes[1].set_title("多项式特征 + 线性回归（能拐弯）")
    axes[1].legend()

    for ax in axes:
        ax.set_xlabel("特征 X（如面积）")
        ax.set_ylabel("目标 y（如房价）")
        ax.grid(True, alpha=0.3)

    plt.tight_layout()
    out_path = os.path.join(OUT, "01_linear_regression.png")
    plt.savefig(out_path, dpi=120, bbox_inches="tight")
    print("对比图已保存到:", out_path)


if __name__ == "__main__":
    main()
