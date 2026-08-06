"""
01. 图像的读取、显示、保存与像素操作
=====================================
最基础也最重要的几个操作。重点理解：
  - OpenCV 读彩色图默认是 **BGR** 顺序（不是 RGB！）
  - 图像在内存里就是一个 numpy 数组，shape = (高, 宽, 通道)
  - 像素坐标是 [y, x]（先行后列），和直觉相反，要特别小心
"""
import os
import cv2

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
PHOTO = os.path.join(OUT, "photo.png")

# ---------- 1) 读取图像 ----------
# imread 第二个参数常用三种：
#   cv2.IMREAD_COLOR    : 彩色，忽略透明通道（默认）  -> 3 通道
#   cv2.IMREAD_GRAYSCALE: 灰度                       -> 1 通道
#   cv2.IMREAD_UNCHANGED: 原样读（含 alpha 透明通道）
img_color = cv2.imread(PHOTO, cv2.IMREAD_COLOR)
img_gray = cv2.imread(PHOTO, cv2.IMREAD_GRAYSCALE)

# 务必判空：路径错了 imread 不会报错，而是返回 None
if img_color is None:
    raise SystemExit(f"读取失败，请先运行 00 脚本生成 {PHOTO}")

# ---------- 2) 图像属性 ----------
print("彩色图 shape:", img_color.shape)   # (高, 宽, 3)
print("灰度图 shape:", img_gray.shape)    # (高, 宽)
print("数据类型 dtype:", img_color.dtype) # uint8 (0~255)
print("总像素数 size:", img_color.size)   # 高*宽*通道

# ---------- 3) 像素访问与修改 ----------
# 注意坐标是 [y, x] = [行, 列]
px = img_color[100, 100]            # 取 (100,100) 处的 BGR 三通道值
print("像素 (100,100) 的 BGR =", px)  # 例如 [B, G, R]

# 只取某通道：img_color[y, x, 0]=B  1=G  2=R
img_color[100, 100] = [255, 255, 255]  # 把这个像素改成白色

# ---------- 4) ROI（感兴趣区域）切片 ----------
# numpy 切片是“视图”，修改会影响原图；这里复制一份
roi = img_color[60:200, 60:220].copy()
cv2.imwrite(os.path.join(OUT, "01_roi.png"), roi)

# 把左上角的一块区域“复制”到右下角
patch = img_color[60:200, 60:220].copy()
img_copy = img_color.copy()
img_copy[280:420, 420:580] = patch   # 形状必须一致
cv2.imwrite(os.path.join(OUT, "01_copy_patch.png"), img_copy)

# ---------- 5) 通道拆分与合并 ----------
b, g, r = cv2.split(img_color)        # 拆成 3 个单通道
merged = cv2.merge((b, g, r))         # 再合并回来，结果与原图相同
# 只保留红色通道（把 B、G 置零）—— 直观体会通道含义
only_red = img_color.copy()
only_red[:, :, 0] = 0   # B 通道清零
only_red[:, :, 1] = 0   # G 通道清零
cv2.imwrite(os.path.join(OUT, "01_only_red_channel.png"), only_red)

# ---------- 6) 保存 ----------
cv2.imwrite(os.path.join(OUT, "01_gray.png"), img_gray)

# ---------- 7) 显示（按任意键继续）----------
cv2.imshow("color", img_color)
cv2.imshow("gray", img_gray)
cv2.imshow("only red channel", only_red)
print("\n弹窗后按任意键关闭...")
cv2.waitKey(0)
cv2.destroyAllWindows()
