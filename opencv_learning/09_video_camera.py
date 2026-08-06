"""
09. 视频 / 摄像头：读取、逐帧处理、保存
====================================
视频本质就是“一连串图片(frame)”。套路：
  cap = cv2.VideoCapture(源)     # 0 表示默认摄像头；也可传视频文件路径
  while True:
      ret, frame = cap.read()    # 一帧帧读
      ... 对 frame 做处理 ...
  cap.release()

本例：打开摄像头，每帧同时显示原图和 Canny 边缘图，并把原始视频存成 .avi。
按 q 退出。如果没有摄像头，把下面的 SOURCE 改成视频文件路径即可。
"""
import os
import cv2

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")

# 设环境变量 OPENCV_NOSHOW=1 可关闭窗口并只跑有限帧（自动化测试用）
SHOW = os.environ.get("OPENCV_NOSHOW") is None
MAX_FRAMES = 10 ** 9 if SHOW else 60

SOURCE = 0          # ← 改成 "xxx.mp4" 就能处理视频文件
cap = cv2.VideoCapture(SOURCE)
if not cap.isOpened():
    raise SystemExit(f"无法打开视频源 {SOURCE!r}（摄像头被占用或不存在；"
                     f"或把 SOURCE 改成视频文件路径）")

# 准备 VideoWriter：fourcc 是编码格式，(宽,高) 必须和帧一致
frame_w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fourcc = cv2.VideoWriter_fourcc(*"XVID")
writer = cv2.VideoWriter(os.path.join(OUT, "09_output.avi"), fourcc, 20.0,
                         (frame_w, frame_h))

print("开始读取... 按 q 退出" if SHOW else "无窗口模式：自动跑 60 帧后退出")
frame_cnt = 0
while True:
    ret, frame = cap.read()
    if not ret:
        print("读取结束（ret=False）")
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    edges = cv2.Canny(gray, 100, 200)
    edges_bgr = cv2.cvtColor(edges, cv2.COLOR_GRAY2BGR)   # 转回3通道才能和原图拼接
    view = cv2.hconcat([frame, edges_bgr])

    writer.write(frame)        # 保存原始帧到视频文件

    if SHOW:
        cv2.imshow("camera | canny edges", view)
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    frame_cnt += 1
    if frame_cnt >= MAX_FRAMES:
        print(f"已达 {MAX_FRAMES} 帧，自动退出")
        break

cap.release()
writer.release()
if SHOW:
    cv2.destroyAllWindows()
print(f"共处理 {frame_cnt} 帧，视频已保存到 output/09_output.avi")
