"""
08. 轮廓检测 + 形状识别（最实用的示例之一）
========================================
轮廓 = 物体的外边界。典型流程：
  彩色图 -> 灰度 -> 二值化 -> findContours -> 逐个分析

本例对 shapes.png 里的几何图形做检测，并用“多边形近似顶点数”判断形状：
  3 顶点=三角形, 4=矩形/正方形, 5=五边形, 很多=圆
"""
import os
import numpy as np
import cv2

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
SHAPES = os.path.join(OUT, "shapes.png")
img = cv2.imread(SHAPES, cv2.IMREAD_COLOR)
if img is None:
    raise SystemExit(f"读取失败，请先运行 00 脚本生成 {SHAPES}")

# 1) 预处理：灰度 + 二值化（图形=白，背景=黑）
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
_, bw = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)

# 2) 找轮廓
# OpenCV 4/5 返回两个值：contours（轮廓点列表）、hierarchy（层级关系）
contours, hierarchy = cv2.findContours(bw, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
print(f"共检测到 {len(contours)} 个外轮廓\n")

# 3) 逐个分析
out = img.copy()
for c in contours:
    area = cv2.contourArea(c)
    if area < 100:          # 过滤掉太小的噪点
        continue

    peri = cv2.arcLength(c, True)                      # 周长
    approx = cv2.approxPolyDP(c, 0.04 * peri, True)    # 多边形近似（顶点变少）
    x, y, w, h = cv2.boundingRect(c)                   # 正外接矩形
    cv2.rectangle(out, (x, y), (x + w, y + h), (0, 255, 255), 2)   # 画外接框

    # 用 moments 求重心
    M = cv2.moments(c)
    cx = int(M["m10"] / M["m00"])
    cy = int(M["m01"] / M["m00"])
    cv2.circle(out, (cx, cy), 4, (0, 0, 255), -1)

    # 4) 按顶点数判断形状
    n = len(approx)
    if n == 3:
        shape = "triangle"
    elif n == 4:
        ratio = w / float(h)
        shape = "square" if 0.9 <= ratio <= 1.1 else "rectangle"
    elif n == 5:
        shape = "pentagon"
    elif n >= 8:
        shape = "circle"
    else:
        shape = f"{n}-gon"

    cv2.putText(out, shape, (x, y - 8), cv2.FONT_HERSHEY_SIMPLEX,
                0.55, (0, 0, 200), 2, cv2.LINE_AA)
    print(f"  {shape:10s} area={area:8.0f}  perimeter={peri:8.1f}  vertices={n}")

# 把所有轮廓用绿色描出来
cv2.drawContours(out, contours, -1, (0, 255, 0), 2)
cv2.imwrite(os.path.join(OUT, "08_contours.png"), out)

cv2.imshow("contours", out)
print("\n弹窗后按任意键关闭...")
cv2.waitKey(0)
cv2.destroyAllWindows()
