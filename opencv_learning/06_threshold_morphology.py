"""
06. 阈值处理 + 形态学运算
======================
【阈值】把灰度图变成“非黑即白”的二值图，是很多检测流程的第一步。
  - threshold        : 全局阈值（手动给定，或用 OTSU 自动求）
  - adaptiveThreshold: 自适应阈值，光照不均时效果更好

【形态学】在二值图上做“膨胀/腐蚀”类操作，用于去噪点、填洞、提边缘：
  - erode 腐蚀 / dilate 膨胀
  - 开运算（先腐蚀后膨胀）去小白点
  - 闭运算（先膨胀后腐蚀）填小黑洞
  - 梯度 = 膨胀 - 腐蚀 ≈ 边缘
"""
import os
import numpy as np
import cv2

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
PHOTO = os.path.join(OUT, "photo.png")
SHAPES = os.path.join(OUT, "shapes.png")

# ---------- 1) 阈值处理（用 photo.png 的灰度图）----------
gray = cv2.cvtColor(cv2.imread(PHOTO), cv2.COLOR_BGR2GRAY)

_, th_bin = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)            # 手动 127
# OTSU 会自动算出最佳阈值，第一个返回值就是它
ret_otsu, th_otsu = cv2.threshold(gray, 0, 255,
                                  cv2.THRESH_BINARY + cv2.THRESH_OTSU)
th_adapt = cv2.adaptiveThreshold(   # 自适应（光照不均时明显更好）
    gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
print(f"OTSU 自动算出的阈值 = {ret_otsu}")

# 三种阈值结果横向拼接存盘对比
def s1(m):
    return cv2.resize(m, (200, 150))
cv2.imwrite(os.path.join(OUT, "06_threshold.png"),
            cv2.hconcat([s1(gray), s1(th_bin), s1(th_otsu), s1(th_adapt)]))

# ---------- 2) 形态学（用 shapes.png）----------
shape_gray = cv2.cvtColor(cv2.imread(SHAPES), cv2.COLOR_BGR2GRAY)
# 白底彩色图形：用 BINARY_INV 让“图形=白、背景=黑”，便于找物体
_, bw = cv2.threshold(shape_gray, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)

kernel = np.ones((5, 5), np.uint8)
erode = cv2.erode(bw, kernel, iterations=1)
dilate = cv2.dilate(bw, kernel, iterations=1)
opening = cv2.morphologyEx(bw, cv2.MORPH_OPEN, kernel)   # 开：去噪点
closing = cv2.morphologyEx(bw, cv2.MORPH_CLOSE, kernel)  # 闭：填洞
gradient = cv2.morphologyEx(bw, cv2.MORPH_GRADIENT, kernel)  # 梯度≈边缘


def small(m):
    return cv2.resize(m, (180, 135))

row = cv2.hconcat([small(bw), small(erode), small(dilate),
                   small(opening), small(closing), small(gradient)])
cv2.imwrite(os.path.join(OUT, "06_morphology.png"), row)

cv2.imshow("bw | erode | dilate | open | close | gradient", row)
print("弹窗后按任意键关闭...")
cv2.waitKey(0)
cv2.destroyAllWindows()
