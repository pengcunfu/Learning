"""
05. 图像滤波（模糊 / 锐化）
=========================
为什么模糊？去噪、降细节、做特效。不同滤波各有用途：
  - 均值 blur         : 简单平均，最基础
  - 高斯 GaussianBlur : 加权平均（中间权重大），最常用，看起来“自然”
  - 中值 medianBlur   : 取中位数，去“椒盐噪声”最有效
  - 双边 bilateralFilter: 保边去噪（磨皮常用，模糊了噪点但边缘清晰）
  - filter2D          : 自己设计卷积核（如锐化、浮雕）
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


# 先给原图人为加点“椒盐噪声”，好对比各滤波器去噪效果
def add_salt_pepper(image, amount=0.02):
    noisy = image.copy()
    h, w = image.shape[:2]
    n = int(h * w * amount)
    ys, xs = np.random.randint(0, h, n), np.random.randint(0, w, n)
    noisy[ys, xs] = 255            # 盐（白点）
    ys, xs = np.random.randint(0, h, n), np.random.randint(0, w, n)
    noisy[ys, xs] = 0              # 椒（黑点）
    return noisy


noisy = add_salt_pepper(img, 0.02)
cv2.imwrite(os.path.join(OUT, "05_noisy.png"), noisy)

blur = cv2.blur(noisy, (7, 7))                 # 均值
gauss = cv2.GaussianBlur(noisy, (7, 7), 0)     # 高斯
median = cv2.medianBlur(noisy, 5)              # 中值 ← 去椒盐最好
bilat = cv2.bilateralFilter(noisy, 9, 75, 75)  # 双边（保边）

# 自定义“锐化核”：中心加大权重，周围减
kernel_sharpen = np.array([[0, -1, 0],
                           [-1, 5, -1],
                           [0, -1, 0]])
sharpen = cv2.filter2D(img, -1, kernel_sharpen)
cv2.imwrite(os.path.join(OUT, "05_sharpen.png"), sharpen)

# 把四张去噪结果横向拼起来对比（统一缩小到同样大小便于观看）
def small(m):
    return cv2.resize(m, (240, 180))

compare = cv2.hconcat([small(noisy), small(blur), small(gauss),
                       small(median), small(bilat)])
cv2.imwrite(os.path.join(OUT, "05_denoise_compare.png"), compare)

cv2.imshow("noisy | blur | gaussian | median | bilateral", compare)
print("弹窗后按任意键关闭...")
cv2.waitKey(0)
cv2.destroyAllWindows()
print("提示：中值滤波(median) 对椒盐噪声效果最好；双边(bilateral) 能保住边缘。")
