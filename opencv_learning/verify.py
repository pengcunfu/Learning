"""
验证脚本（非学习内容）：以“无窗口”方式把 01~10 全跑一遍，确认都能正常运行。
用法：  python verify.py
"""
import os
os.environ["OPENCV_NOSHOW"] = "1"      # 关闭 09/10 的窗口并限帧

import sys
import cv2
import runpy

# 屏蔽 01~08 里的弹窗调用，避免 waitKey 阻塞
cv2.imshow = lambda *a, **k: None
cv2.waitKey = lambda *a, **k: 0
cv2.destroyAllWindows = lambda *a, **k: None
cv2.destroyWindow = lambda *a, **k: None

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
files = sorted(f for f in os.listdir(BASE_DIR)
               if f[0].isdigit() and f.endswith(".py"))

ok, fail = 0, 0
for f in files:
    path = os.path.join(BASE_DIR, f)
    print(f"\n==== 运行 {f} ====")
    try:
        runpy.run_path(path, run_name="__main__")
        print("    -> OK")
        ok += 1
    except SystemExit:
        print("    -> OK (正常退出)")
        ok += 1
    except Exception as e:
        print(f"    -> 出错: {type(e).__name__}: {e}")
        fail += 1

print(f"\n结果：成功 {ok} 个，失败 {fail} 个")
sys.exit(0 if fail == 0 else 1)
