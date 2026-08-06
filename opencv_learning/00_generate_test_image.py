"""
00. 生成测试素材
=================
本套示例的所有测试图片都由这个脚本生成，**不依赖任何外部图片**。
请先运行本脚本，它会在 output/ 目录下生成：
  - photo.png : 一张内容丰富的“模拟照片”，用于颜色、模糊、边缘等练习
  - shapes.png: 白底彩色实心几何图形，用于轮廓检测与形状识别

你也会学到：如何用 numpy 创建图像、如何画基本图形、如何加噪声。
"""
import os
import numpy as np
import cv2

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(BASE_DIR, "output")
os.makedirs(OUT_DIR, exist_ok=True)


def make_photo():
    """生成一张内容丰富的彩色图：渐变背景 + 几个图形 + 文字 + 少量噪声。"""
    h, w = 480, 640
    # 1) 用 numpy 直接做“横向蓝绿渐变”背景
    #    np.linspace 生成 0~255，再广播成整张图
    grad = np.linspace(0, 255, w, dtype=np.uint8)        # 形状 (w,)
    bg = np.zeros((h, w, 3), dtype=np.uint8)
    bg[:, :, 0] = 255 - grad       # B 通道：左高右低
    bg[:, :, 1] = grad             # G 通道：左低右高
    bg[:, :, 2] = 80               # R 通道：固定

    # 2) 画几个彩色实心图形，丰富内容
    cv2.rectangle(bg, (60, 60), (220, 200), (0, 0, 220), -1)        # 红色实心矩形
    cv2.circle(bg, (440, 150), 70, (0, 220, 0), -1)                 # 绿色实心圆
    cv2.ellipse(bg, (320, 360), (90, 40), 30, 0, 360, (0, 200, 255), -1)  # 橙色椭圆

    # 3) 加文字
    cv2.putText(bg, "OpenCV Demo", (140, 280), cv2.FONT_HERSHEY_SIMPLEX,
                1.2, (255, 255, 255), 2, cv2.LINE_AA)

    # 4) 加少量高斯噪声，模拟真实照片（也让后面的去噪演示更直观）
    noise = np.random.normal(0, 18, bg.shape).astype(np.int16)
    photo = np.clip(bg.astype(np.int16) + noise, 0, 255).astype(np.uint8)

    cv2.imwrite(os.path.join(OUT_DIR, "photo.png"), photo)
    print("[OK] 已生成 output/photo.png")


def make_shapes():
    """生成白底彩色实心几何图形，供轮廓检测使用。"""
    h, w = 480, 640
    img = np.full((h, w, 3), 255, dtype=np.uint8)  # 白底

    # 不同颜色、不同形状，且彼此不重叠，方便逐个检测
    cv2.rectangle(img, (60, 80), (200, 220), (180, 80, 40), -1)     # 棕色矩形
    cv2.circle(img, (340, 150), 70, (40, 40, 200), -1)              # 红色圆
    # 手工画一个三角形（用 fillConvexPoly / polylines）
    tri = np.array([[490, 230], [590, 230], [540, 90]], dtype=np.int32)
    cv2.fillConvexPoly(img, tri, (40, 180, 40))                     # 绿色三角形
    # 五边形
    pentagon = np.array([
        [120, 360], [60, 410], [85, 480], [155, 480], [180, 410]
    ], dtype=np.int32)
    cv2.fillConvexPoly(img, pentagon, (200, 100, 180))             # 紫色五边形
    # 圆环（空心）
    cv2.circle(img, (340, 400), 55, (0, 0, 0), 4)                  # 黑色空心圆

    cv2.imwrite(os.path.join(OUT_DIR, "shapes.png"), img)
    print("[OK] 已生成 output/shapes.png")


if __name__ == "__main__":
    make_photo()
    make_shapes()
    print("\n素材已就绪，可以开始学习 01 ~ 10 了。")

    # 顺便展示一下
    for name in ("photo.png", "shapes.png"):
        img = cv2.imread(os.path.join(OUT_DIR, name))
        cv2.imshow(name, img)
    print("按任意键关闭预览窗口...")
    cv2.waitKey(0)
    cv2.destroyAllWindows()
