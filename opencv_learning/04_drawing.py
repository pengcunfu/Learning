"""
04. 绘图：线、矩形、圆、椭圆、多边形、文字 + 鼠标交互
=====================================================
绘图函数都很像：传入 图像、坐标、颜色(BGR)、线宽。
  - 线宽填 -1 表示“实心填充”
  - 颜色都是 (B, G, R)
  - putText 只支持英文/数字，中文要用 PIL 等库
"""
import os
import numpy as np
import cv2

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")

# ---------- 静态绘图 ----------
canvas = np.full((480, 640, 3), 255, np.uint8)  # 白底画布

cv2.line(canvas, (0, 0), (640, 480), (255, 0, 0), 3)        # 蓝色对角线
cv2.rectangle(canvas, (50, 50), (200, 150), (0, 200, 0), 3) # 绿色空心矩形
cv2.rectangle(canvas, (220, 50), (300, 130), (0, 200, 0), -1) # 绿色实心矩形
cv2.circle(canvas, (380, 100), 50, (0, 0, 220), -1)         # 红色实心圆
cv2.ellipse(canvas, (520, 110), (80, 30), 20, 0, 360, (255, 0, 255), 2)

# 多边形（注意：第二个参数是“顶点列表的列表”）
pentagon = np.array([[100, 400], [200, 350], [300, 420],
                     [250, 470], [150, 460]], np.int32)
cv2.polylines(canvas, [pentagon], True, (0, 128, 255), 2)   # True=自动闭合

# 箭头线 + 文字
cv2.arrowedLine(canvas, (400, 350), (580, 450), (0, 0, 0), 2, tipLength=0.05)
cv2.putText(canvas, "Hello OpenCV", (20, 30), cv2.FONT_HERSHEY_SIMPLEX,
            1.0, (0, 0, 0), 2, cv2.LINE_AA)

cv2.imwrite(os.path.join(OUT, "04_drawing.png"), canvas)

cv2.imshow("drawing", canvas)
print("弹窗后按任意键关闭（想玩鼠标交互，看代码末尾）...")
cv2.waitKey(0)
cv2.destroyAllWindows()


# ---------- 鼠标交互示例（默认不运行）----------
# 把下面的 mouse_demo() 取消注释运行：在弹出的窗口里“左键点击画彩色圆”，按 q 退出。
def mouse_demo():
    img = np.full((480, 640, 3), 255, np.uint8)

    def on_event(event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:           # 左键按下
            color = tuple(int(c) for c in np.random.randint(0, 255, 3))
            cv2.circle(img, (x, y), 20, color, -1)

    cv2.namedWindow("mouse")
    cv2.setMouseCallback("mouse", on_event)
    print("在 'mouse' 窗口左键点击画圆，按 q 退出")
    while True:
        cv2.imshow("mouse", img)
        if cv2.waitKey(20) & 0xFF == ord("q"):
            break
    cv2.destroyAllWindows()

# mouse_demo()
