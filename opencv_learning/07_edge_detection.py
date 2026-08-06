"""
07. 边缘检测
==========
边缘 = 像素值剧烈变化的地方。核心思路都是“求导/求梯度”。
  - Sobel  : 一阶导数，分 x / y 方向，可合成
  - Scharr : Sobel 的高精度版本（小核但方向准）
  - Laplacian: 二阶导数，对噪声敏感
  - Canny  : 最常用的“一站式”边缘检测器，自带去噪+双阈值+连接，效果最好
"""
import os
import numpy as np
import cv2

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
PHOTO = os.path.join(OUT, "photo.png")
gray = cv2.cvtColor(cv2.imread(PHOTO), cv2.COLOR_BGR2GRAY)
if gray is None:
    raise SystemExit(f"读取失败，请先运行 00 脚本生成 {PHOTO}")

# 先高斯模糊一下，降低噪声对求导的干扰（Canny 内部也会做）
blur = cv2.GaussianBlur(gray, (3, 3), 0)

# ---------- 1) Sobel ----------
# CV_64F 用浮点避免梯度截断；dx=1,dy=0 表示求 x 方向导数
sobel_x = cv2.Sobel(blur, cv2.CV_64F, 1, 0, ksize=3)
sobel_y = cv2.Sobel(blur, cv2.CV_64F, 0, 1, ksize=3)
sobel_x = np.uint8(np.absolute(sobel_x))
sobel_y = np.uint8(np.absolute(sobel_y))
sobel = cv2.bitwise_or(sobel_x, sobel_y)   # 合成总边缘

# ---------- 2) Laplacian（二阶导数）----------
lap = cv2.Laplacian(blur, cv2.CV_64F, ksize=3)
lap = np.uint8(np.absolute(lap))

# ---------- 3) Canny（重点）----------
canny_manual = cv2.Canny(blur, 100, 200)   # 手动给低阈值100、高阈值200

# 自动估算 Canny 阈值的经验公式（基于图中位像素值）
v = np.median(blur)
sigma = 0.33
lower = int(max(0, (1.0 - sigma) * v))
upper = int(min(255, (1.0 + sigma) * v))
canny_auto = cv2.Canny(blur, lower, upper)
print(f"自动 Canny 阈值：low={lower}, high={upper}")


def small(m):
    return cv2.resize(m, (240, 180))

row = cv2.hconcat([small(gray), small(sobel), small(lap),
                   small(canny_manual), small(canny_auto)])
cv2.imwrite(os.path.join(OUT, "07_edges.png"), row)

cv2.imshow("gray | sobel | laplacian | canny(100,200) | canny(auto)", row)
print("弹窗后按任意键关闭...")
cv2.waitKey(0)
cv2.destroyAllWindows()
print("提示：调 Canny 的两个阈值——低阈值更小→边缘更多（但噪点也多）；反之边缘更干净。")
