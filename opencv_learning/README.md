# OpenCV 学习示例

一套从入门到进进的 OpenCV（Python）示例，每个文件聚焦一个主题，**可独立运行**，带详细中文注释。

## 环境准备

```bash
# 安装 OpenCV（已安装可跳过）
python -m pip install opencv-python -i https://pypi.tuna.tsinghua.edu.cn/simple
```

> 本套示例只需要 `opencv-python` 和 `numpy`。所有测试图片都由 `00` 脚本生成，**不需要自己找图片**。

## 学习路线（建议按顺序）

| 序号 | 文件 | 主题 |
|----|------|------|
| 00 | `00_generate_test_image.py` | 生成测试素材（先运行这个！） |
| 01 | `01_read_write_image.py` | 图像读取、显示、保存、像素操作 |
| 02 | `02_basic_operations.py` | 裁剪、缩放、旋转、翻转、平移 |
| 03 | `03_color_spaces.py` | 颜色空间转换、HSV 颜色提取 |
| 04 | `04_drawing.py` | 画线、矩形、圆、多边形、文字、鼠标交互 |
| 05 | `05_blur_filter.py` | 各类模糊滤波、自定义卷积核 |
| 06 | `06_threshold_morphology.py` | 阈值处理、形态学运算 |
| 07 | `07_edge_detection.py` | Sobel、Laplacian、Canny 边缘检测 |
| 08 | `08_contours.py` | 轮廓检测与形状识别 |
| 09 | `09_video_camera.py` | 摄像头/视频读取、处理、保存 |
| 10 | `10_face_detection.py` | 人脸/眼睛检测（Haar 级联） |

## 运行方式

```bash
cd D:\Learning\opencv_learning
python 00_generate_test_image.py   # 先运行，生成 output/ 下的测试图片
python 01_read_write_image.py      # 之后任意顺序运行
```

- 每个示例会用 `cv2.imshow` 弹出窗口，**按任意键**关闭窗口、继续程序。
- 处理结果也会保存到 `output/` 目录，方便事后查看。
- 视频示例（09、10）按 `q` 键退出。

## 学习建议

1. 先通读一遍代码和注释，理解每一步在做什么。
2. **改参数**：改阈值、改核大小、改颜色范围，观察结果变化——这是最有效的学法。
3. 把自己的照片放到 `output/` 目录，把示例里的 `photo.png` 换成你的图试试效果。
