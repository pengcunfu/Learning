"""下载 Haar 级联模型（多镜像源，自动选可用的）。运行: python _download_haar.py"""
import os
import urllib.request

BASE = os.path.dirname(os.path.abspath(__file__))
DST = os.path.join(BASE, "models")
os.makedirs(DST, exist_ok=True)

FILES = ["haarcascade_frontalface_default.xml", "haarcascade_eye.xml"]
MIRRORS = [
    "https://raw.githubusercontent.com/opencv/opencv/4.x/haarcascades/",
    "https://ghfast.top/https://raw.githubusercontent.com/opencv/opencv/4.x/haarcascades/",
    "https://ghproxy.net/https://raw.githubusercontent.com/opencv/opencv/4.x/haarcascades/",
    "https://gitee.com/mirrors/opencv/raw/4.x/haarcascades/",
]

for f in FILES:
    p = os.path.join(DST, f)
    if os.path.exists(p) and os.path.getsize(p) > 1000:
        with open(p, "rb") as fp:
            if fp.read(5) == b"<?xml":
                print(f"[skip] {f} 已存在且有效")
                continue
    done = False
    for base in MIRRORS:
        url = base + f
        try:
            data = urllib.request.urlopen(url, timeout=30).read()
            if data[:5] == b"<?xml":
                with open(p, "wb") as fp:
                    fp.write(data)
                print(f"[ok]   {f}  <-  {base}  ({len(data)} bytes)")
                done = True
                break
            else:
                print(f"[no]   非 xml 内容: {base}")
        except Exception as e:
            print(f"[err]  {base}  {e}")
    if not done:
        print(f"[FAIL] {f} 所有镜像源均失败")

print("完成")
