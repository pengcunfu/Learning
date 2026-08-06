"""
03. 颜色空间转换 + HSV 颜色提取
==============================
为什么需要颜色空间？
  - Gray（灰度）：去掉颜色信息，常用于边缘/轮廓/人脸检测，更快更稳。
  - HSV：H(色调) S(饱和度) V(明度)。最大优点是“把颜色和亮度分开”，
         所以按颜色找物体（比如“找红色的球”）用 HSV 比用 BGR 容易得多。
  - RGB：给 matplotlib / PIL 等其它库用时，要把 BGR 转成 RGB。

重点演示：用 HSV 阈值把图片里的“绿色物体”抠出来——这是最实用的技巧之一。
"""
import os
import numpy as np
import cv2

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
PHOTO = os.path.join(OUT, "photo.png")
img = cv2.imread(PHOTO, cv2.IMREAD_COLOR)
if img is None:
    raise SystemExit(f"读取失败，请先运行 00 脚本生成 {PHOTO}")

# ---------- 1) BGR <-> Gray ----------
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
cv2.imwrite(os.path.join(OUT, "03_gray.png"), gray)

# ---------- 2) BGR <-> RGB（给其它库用）----------
rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

# ---------- 3) BGR -> HSV ----------
hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

# ---------- 4) 用 HSV 提取“绿色”物体 ----------
# OpenCV 里 H 范围是 0~179（不是 0~359），S/V 都是 0~255
# 绿色 H 大约在 35~85 之间
lower_green = np.array([35, 50, 50])
upper_green = np.array([85, 255, 255])
mask = cv2.inRange(hsv, lower_green, upper_green)        # 二值掩膜：绿处=255，否则=0

# 位运算 and：用掩膜把原图里的绿色部分抠出来，其它变黑
green_only = cv2.bitwise_and(img, img, mask=mask)

cv2.imwrite(os.path.join(OUT, "03_green_mask.png"), mask)
cv2.imwrite(os.path.join(OUT, "03_green_only.png"), green_only)

# 小技巧：如果想抠“红色”，由于红色在 HSV 两端（H≈0 和 H≈179），
# 要做两个范围再 or 起来：
mask_red1 = cv2.inRange(hsv, np.array([0, 50, 50]), np.array([10, 255, 255]))
mask_red2 = cv2.inRange(hsv, np.array([170, 50, 50]), np.array([179, 255, 255]))
mask_red = cv2.bitwise_or(mask_red1, mask_red2)
red_only = cv2.bitwise_and(img, img, mask=mask_red)
cv2.imwrite(os.path.join(OUT, "03_red_only.png"), red_only)

# ---------- 显示 ----------
cv2.imshow("original", img)
cv2.imshow("green mask", mask)
cv2.imshow("green only", green_only)
print("弹窗后按任意键关闭...")
cv2.waitKey(0)
cv2.destroyAllWindows()

# 思考题：把 lower_green/upper_green 改一下，试试抠出橙色椭圆（H≈10~25）。
