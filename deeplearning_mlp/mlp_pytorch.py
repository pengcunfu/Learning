"""
MLP（多层感知机）实现 —— PyTorch 版本
=====================================

PyTorch 把上一份代码里手写的“前向/反向/梯度更新”全部封装好了：
    - nn.Linear：一个全连接层（含权重 W 和偏置 b）
    - 模型自动求导（autograd），不用手写反向传播
    - optim.Adam：自适应优化器，自动更新参数
    - DataLoader：自动分批(batch)喂数据

任务：MNIST 手写数字识别（0~9 共 10 类），输入是 28x28 灰度图。

运行：python mlp_pytorch.py
依赖：pip install torch torchvision
首次运行会自动下载 MNIST 数据集到 ./data 目录。
"""

import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader
from torchvision import datasets, transforms


# ----------------------------------------------------------------------
# 1. 定义模型：用 nn.Module 描述网络结构
# ----------------------------------------------------------------------
class MLP(nn.Module):
    def __init__(self, input_dim=28 * 28, hidden_dim=128, num_classes=10):
        super().__init__()
        # flatten：把 28x28 图像拉平成 784 维向量
        self.flatten = nn.Flatten()
        self.net = nn.Sequential(
            nn.Linear(input_dim, hidden_dim),  # 第 1 个全连接层
            nn.ReLU(),                          # 非线性激活
            nn.Linear(hidden_dim, hidden_dim),  # 第 2 个全连接层
            nn.ReLU(),
            nn.Linear(hidden_dim, num_classes), # 输出层：10 类
        )

    def forward(self, x):
        x = self.flatten(x)
        return self.net(x)


def main():
    # 自动选 GPU，没有就用 CPU
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    print(f"使用设备: {device}")

    # ------------------------------------------------------------------
    # 2. 加载数据
    # ------------------------------------------------------------------
    # ToTensor：把图片转成 [0,1] 的张量；Normalize：标准化到均值0、方差1
    transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.1307,), (0.3081,)),
    ])

    train_set = datasets.MNIST("./data", train=True, download=True,
                               transform=transform)
    test_set = datasets.MNIST("./data", train=False, download=True,
                              transform=transform)

    # DataLoader 负责按 batch 迭代，并自动打乱(shuffle)训练集
    train_loader = DataLoader(train_set, batch_size=64, shuffle=True)
    test_loader = DataLoader(test_set, batch_size=1000, shuffle=False)

    # ------------------------------------------------------------------
    # 3. 实例化模型、损失函数、优化器
    # ------------------------------------------------------------------
    model = MLP().to(device)
    criterion = nn.CrossEntropyLoss()        # 多分类用交叉熵（内含 softmax）
    optimizer = optim.Adam(model.parameters(), lr=1e-3)

    # ------------------------------------------------------------------
    # 4. 训练循环
    # ------------------------------------------------------------------
    epochs = 5
    for epoch in range(epochs):
        model.train()                         # 切到训练模式（影响 Dropout/BN）
        running_loss = 0.0

        for batch_idx, (data, target) in enumerate(train_loader):
            data, target = data.to(device), target.to(device)

            optimizer.zero_grad()             # ① 清空上一步的梯度
            output = model(data)              # ② 前向传播
            loss = criterion(output, target)  # ③ 计算损失
            loss.backward()                   # ④ 反向传播（自动求梯度）
            optimizer.step()                  # ⑤ 用梯度更新参数

            running_loss += loss.item() * data.size(0)

        # 每个 epoch 在测试集上评估
        avg_loss = running_loss / len(train_loader.dataset)
        test_acc = evaluate(model, test_loader, device)
        print(f"epoch {epoch + 1}/{epochs}  loss={avg_loss:.4f}  test_acc={test_acc:.4f}")

    # ------------------------------------------------------------------
    # 5. 保存模型权重，方便以后加载复用
    # ------------------------------------------------------------------
    torch.save(model.state_dict(), "mnist_mlp.pth")
    print("模型已保存到 mnist_mlp.pth")


def evaluate(model, test_loader, device):
    """在测试集上计算准确率"""
    model.eval()                              # 切到评估模式
    correct = 0
    with torch.no_grad():                     # 评估不需要算梯度，省内存、加速
        for data, target in test_loader:
            data, target = data.to(device), target.to(device)
            output = model(data)
            pred = output.argmax(dim=1)       # 取概率最大的类别
            correct += (pred == target).sum().item()
    return correct / len(test_loader.dataset)


if __name__ == "__main__":
    main()
