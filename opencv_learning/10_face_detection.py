"""
10. 人脸 / 眼睛检测（Haar 级联分类器）
===================================
经典方法：用预训练的 Haar 特征分类器在图里“滑动扫描”找人脸。
虽然现在有更准的深度学习方法，但 Haar 轻量、无需 GPU，至今很实用。

模型文件（xml）位于 models/ 目录；若不存在会自动从 OpenCV 官方仓库下载。
"""
import os
import cv2

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE_DIR, "output")
PHOTO = os.path.join(OUT, "photo.png")
SHOW = os.environ.get("OPENCV_NOSHOW") is None


def load_cascade(filename):
    """优先用本地 models/，没有则从多个镜像源尝试下载（带 XML 头校验）。"""
    local = os.path.join(BASE_DIR, "models", filename)
    if os.path.exists(local) and os.path.getsize(local) > 1000:
        with open(local, "rb") as fp:
            if fp.read(5) == b"<?xml":            # 校验是真 xml 而不是 404 页面
                return cv2.CascadeClassifier(local)

    import urllib.request
    os.makedirs(os.path.join(BASE_DIR, "models"), exist_ok=True)
    mirrors = [
        "https://raw.githubusercontent.com/opencv/opencv/4.x/haarcascades/",
        "https://ghfast.top/https://raw.githubusercontent.com/opencv/opencv/4.x/haarcascades/",
        "https://gitee.com/mirrors/opencv/raw/4.x/haarcascades/",
    ]
    for base in mirrors:
        url = base + filename
        print(f"尝试下载: {url}")
        try:
            data = urllib.request.urlopen(url, timeout=30).read()
            if data[:5] == b"<?xml":
                with open(local, "wb") as fp:
                    fp.write(data)
                return cv2.CascadeClassifier(local)
        except Exception as e:
            print(f"  失败: {e}")
    raise SystemExit(
        f"无法获取 {filename}。请手动下载放到 models/ 目录（可运行 _download_haar.py 辅助）")


face_cascade = load_cascade("haarcascade_frontalface_default.xml")
eye_cascade = load_cascade("haarcascade_eye.xml")


def detect(img):
    """在 img 上检测人脸（蓝框）+ 眼睛（绿框），返回 (图, 人脸数)。"""
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # 参数：图、scaleFactor(每次缩小多少)、minNeighbors(合并重叠框的阈值)
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.3, minNeighbors=5)
    for (x, y, w, h) in faces:
        cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 2)
        roi_gray = gray[y:y + h, x:x + w]      # 只在“脸”的区域内找眼睛，又快又准
        eyes = eye_cascade.detectMultiScale(roi_gray)
        for (ex, ey, ew, eh) in eyes:
            cv2.rectangle(img, (x + ex, y + ey), (x + ex + ew, y + ey + eh),
                          (0, 255, 0), 2)
    return img, len(faces)


# ---------- 优先用摄像头实时检测；没有摄像头就退回静态图 ----------
cap = cv2.VideoCapture(0)
if cap.isOpened():
    print("摄像头已打开，实时检测中，按 q 退出")
    max_frames = 10 ** 9 if SHOW else 60
    cnt = 0
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        frame, n = detect(frame)
        if SHOW:
            cv2.imshow("face detection", frame)
            if cv2.waitKey(1) & 0xFF == ord("q"):
                break
        cnt += 1
        if cnt >= max_frames:
            break
    cap.release()
    if SHOW:
        cv2.destroyAllWindows()
    print(f"实时检测结束，共处理 {cnt} 帧")
else:
    # 无摄像头：对静态图检测（photo.png 大概率没人脸，只是演示流程能跑通）
    print("摄像头不可用，改为对 output/photo.png 做静态检测")
    img = cv2.imread(PHOTO)
    if img is None:
        raise SystemExit(f"读取失败，请先运行 00 脚本生成 {PHOTO}")
    out, n = detect(img)
    cv2.imwrite(os.path.join(OUT, "10_faces.png"), out)
    print(f"检测到 {n} 张人脸（photo.png 是合成的，可能为 0；换成真人照片即可）")
    if SHOW:
        cv2.imshow("face detection", out)
        print("按任意键关闭...")
        cv2.waitKey(0)
        cv2.destroyAllWindows()
