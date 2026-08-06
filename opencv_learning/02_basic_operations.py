"""
02. 几何基本操作：裁剪、缩放、旋转、翻转、平移
==============================================
这些都是对图像做“空间变换”。核心 API：
  - resize  : 缩放（可指定尺寸或缩放因子，可选插值方式）
  - warpAffine + 变换矩阵 : 平移、旋转
  - flip    : 翻转
记住：所有变换返回的都是“新图像”，不会改变原图。
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
h, w = img.shape[:2]

# ---------- 1) 裁剪：本质就是 numpy 切片 ----------
crop = img[80:380, 120:520].copy()    # [y1:y2, x1:x2]
cv2.imwrite(os.path.join(OUT, "02_crop.png"), crop)

# ---------- 2) 缩放 ----------
# 方式 A：直接指定目标尺寸
resized_a = cv2.resize(img, (300, 200))  # 注意参数是 (宽, 高)
# 方式 B：用缩放因子 fx, fy（更常用，保持比例）
resized_b = cv2.resize(img, None, fx=0.5, fy=0.5, interpolation=cv2.INTER_AREA)
# 插值方式选择经验：
#   缩小用 INTER_AREA  （避免摩尔纹）
#   放大用 INTER_CUBIC / INTER_LINEAR
#   实时/最快用 INTER_NEAREST
cv2.imwrite(os.path.join(OUT, "02_resize_half.png"), resized_b)

# ---------- 3) 翻转 ----------
flip_h = cv2.flip(img, 1)   # 1 = 水平翻转（左右镜像）
flip_v = cv2.flip(img, 0)   # 0 = 垂直翻转（上下颠倒）
flip_both = cv2.flip(img, -1)  # -1 = 水平+垂直

# ---------- 4) 平移 ----------
# 用 2x3 仿射矩阵：[[1,0,dx],[0,1,dy]] 表示沿 x 移 dx、沿 y 移 dy
dx, dy = 80, 40
M_translate = np.float32([[1, 0, dx], [0, 1, dy]])
translated = cv2.warpAffine(img, M_translate, (w, h))
cv2.imwrite(os.path.join(OUT, "02_translate.png"), translated)

# ---------- 5) 旋转 ----------
# 围绕中心 (w/2, h/2) 逆时针旋转 45°，并缩放 0.7
center = (w / 2, h / 2)
M_rotate = cv2.getRotationMatrix2D(center, angle=45, scale=0.7)
rotated = cv2.warpAffine(img, M_rotate, (w, h))
cv2.imwrite(os.path.join(OUT, "02_rotate.png"), rotated)

# ---------- 6) 图像拼接（便于对比查看）----------
# vconcat 垂直拼接、hconcat 水平拼接，要求对应维度相同
row1 = cv2.hconcat([cv2.resize(img, (240, 180)),
                    cv2.resize(flip_h, (240, 180)),
                    cv2.resize(flip_v, (240, 180))])
cv2.imwrite(os.path.join(OUT, "02_flips.png"), row1)

# ---------- 显示 ----------
cv2.imshow("crop", crop)
cv2.imshow("rotated 45", rotated)
print("弹窗后按任意键关闭...")
cv2.waitKey(0)
cv2.destroyAllWindows()
