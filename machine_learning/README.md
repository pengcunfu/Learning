# 机器学习基础示例（scikit-learn）

一套面向入门者的机器学习示例（Python + scikit-learn），
每个文件聚焦一个算法或主题，**可独立运行**，带详细中文注释。
所有数据都由代码合成（或使用 sklearn 自带数据集），**不需要联网下载**。

> 搭配已有的 `deeplearning_mlp`（神经网络）学习更佳：
> 本目录学“经典机器学习”，`deeplearning_mlp` 学“深度学习”。
> 两者本质是同一套思想：从数据里学规律 → 预测新样本。

## 环境准备

```bash
pip install numpy matplotlib scikit-learn
```

## 学习路线（建议按顺序）

| 序号 | 文件 | 主题 | 类型 |
|------|------|------|------|
| 00 | `00_generate_datasets.py` | 四种合成数据集总览 | 工具 |
| 01 | `01_linear_regression.py` | 线性回归：预测连续数值 | 监督·回归 |
| 02 | `02_logistic_regression.py` | 逻辑回归：分类 + 概率输出 | 监督·分类 |
| 03 | `03_decision_tree.py` | 决策树：可解释的 if-else 规则 | 监督·分类 |
| 04 | `04_random_forest.py` | 随机森林：集成学习对抗过拟合 | 监督·分类 |
| 05 | `05_svm.py` | SVM：最大间隔 + 核函数 | 监督·分类 |
| 06 | `06_kmeans.py` | K-Means：无监督聚类 | 无监督 |
| 07 | `07_pca.py` | PCA：高维数据降维与可视化 | 无监督 |
| 08 | `08_model_evaluation.py` | 过拟合、交叉验证、模型评估 | 通用技能 |

## 运行方式

```bash
cd D:\Learning\machine_learning
python 00_generate_datasets.py   # 先看数据集长什么样
python 01_linear_regression.py   # 之后任意顺序运行
```

- 每个脚本会打印关键数值（权重、准确率、R²、inertia 等），并把图保存到 `output/` 目录。
- 结果图默认不弹出窗口，直接看 `output/*.png` 即可。

## 每个示例的学习要点

| 文件 | 核心概念 |
|------|----------|
| 01 | y = wx + b、MSE、R²、特征工程（多项式特征） |
| 02 | Sigmoid、概率输出、决策边界、混淆矩阵 |
| 03 | 基尼不纯度、if-else 规则、max_depth、可解释性 |
| 04 | 集成学习、Bagging、降低方差 |
| 05 | 最大间隔、支持向量、核函数（linear / RBF） |
| 06 | 无监督学习、inertia、肘部法则选 K |
| 07 | 高维数据可视化、主成分、方差解释率 |
| 08 | 训练/测试划分、过拟合曲线、K 折交叉验证 |

## 学习建议

1. 先通读代码注释，再看输出图，最后自己跑一遍。
2. **改参数**：改 `max_depth`、`noise`、`n_estimators`、`K`，观察曲线和准确率怎么变——这是最有效的学法。
3. 思考“换一个算法会怎样”：比如把 02 的数据放到 05 的 SVM 上跑跑看。
4. 想深入神经网络时，回到 `deeplearning_mlp` 看同样的问题怎么用深度学习解决。

## 下一步

- 数据预处理：标准化、归一化、缺失值处理、类别特征编码
- 更多评估指标：精确率 / 召回率 / F1（类别不平衡时准确率会骗人）
- 更多算法：KNN、朴素贝叶斯、梯度提升（XGBoost / LightGBM）
- 深度学习：CNN（图像）、RNN / Transformer（序列）——可以结合 `opencv_learning` 做实战
