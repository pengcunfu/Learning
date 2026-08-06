"""从下载好的 opencv-python 4.x wheel 里抽取 Haar 模型到 models/ 目录。"""
import glob
import zipfile
import os
import shutil

dst = r"D:/Learning/opencv_learning/models"
os.makedirs(dst, exist_ok=True)
whls = glob.glob(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                              "_whl_dl", "*.whl"))
assert whls, "没找到 wheel，请先 pip download"
print("wheel:", os.path.basename(whls[0]))

targets = {"haarcascade_frontalface_default.xml", "haarcascade_eye.xml"}
z = zipfile.ZipFile(whls[0])
have = {os.path.basename(n) for n in z.namelist()}
print("wheel 中包含的 haar 模型:",
      sorted(n for n in have if n.startswith("haarcascade")))

for n in z.namelist():
    base = os.path.basename(n)
    if base in targets:
        with z.open(n) as src, open(os.path.join(dst, base), "wb") as out:
            shutil.copyfileobj(src, out)
        print("已抽取 ->", base, os.path.getsize(os.path.join(dst, base)), "bytes")
print("完成")
