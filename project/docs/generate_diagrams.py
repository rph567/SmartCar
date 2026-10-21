# -*- coding: utf-8 -*-
import os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, FancyBboxPatch, FancyArrowPatch, Circle
from matplotlib.gridspec import GridSpec
from matplotlib.font_manager import FontProperties

BASE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE, "images")
os.makedirs(OUT, exist_ok=True)

FONT_PATH = r"C:\Windows\Fonts\simhei.ttf"
FONT = FontProperties(fname=FONT_PATH)
plt.rcParams["axes.unicode_minus"] = False

BLUE = "#2f5597"
LIGHT_BLUE = "#eaf2fb"
GREEN = "#38761d"
LIGHT_GREEN = "#d9ead3"
YELLOW = "#bf9000"
LIGHT_YELLOW = "#fff2cc"
RED = "#c00000"
LIGHT_RED = "#fce5cd"
GRAY = "#666666"


def save(fig, name):
    path = os.path.join(OUT, name)
    fig.savefig(path, dpi=180, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    print("saved:", path)


def box(ax, x, y, w, h, text, fc=LIGHT_BLUE, ec=BLUE, lw=1.6, fs=11, ls="-"):
    patch = FancyBboxPatch(
        (x - w / 2.0, y - h / 2.0), w, h,
        boxstyle="round,pad=0.02,rounding_size=0.08",
        fc=fc, ec=ec, lw=lw, linestyle=ls, zorder=3
    )
    ax.add_patch(patch)
    ax.text(x, y, text, ha="center", va="center", fontproperties=FONT,
            fontsize=fs, zorder=4, linespacing=1.25)
    return patch


def arrow(ax, x1, y1, x2, y2, color=GRAY, lw=1.8, ls="-", rad=0.0):
    p = FancyArrowPatch(
        (x1, y1), (x2, y2), arrowstyle="-|>", mutation_scale=14,
        color=color, lw=lw, linestyle=ls,
        connectionstyle="arc3,rad=%s" % rad, zorder=2
    )
    ax.add_patch(p)


def make_track(h=120, w=188, top=0, center_amp=18.0, center_phase=22.0,
               half_base=42.0, half_grow=8.0):
    img = np.full((h, w), 80, dtype=np.uint8)
    for y in range(h):
        if y < top:
            continue
        c = 94.0 + center_amp * np.sin(y / center_phase)
        half = half_base + half_grow * (y - top) / max(1.0, (h - 1 - top))
        x0 = int(round(c - half))
        x1 = int(round(c + half))
        x0 = max(0, x0)
        x1 = min(w - 1, x1)
        img[y, max(0, x0 - 3):x0] = 20
        img[y, x0:x1 + 1] = 230
        img[y, x1 + 1:min(w, x1 + 4)] = 20
    return img


def make_binary_track(h=100, w=152, top=25, center_amp=18.0, center_phase=18.0,
                      half_base=34.0, half_grow=12.0):
    img = np.zeros((h, w), dtype=np.uint8)
    for y in range(top, h):
        c = 76.0 + center_amp * np.sin((y - top) / center_phase)
        half = half_base + half_grow * (y - top) / max(1.0, (h - 1 - top))
        x0 = max(0, int(round(c - half)))
        x1 = min(w - 1, int(round(c + half)))
        img[y, x0:x1 + 1] = 1
    return img


def longest_white_column(img):
    h, w = img.shape
    best_num, best_col, best_end = -1, 0, 0
    for x in range(w):
        y = h - 1
        num = 0
        while y >= 0 and img[y, x] == 1:
            num += 1
            y -= 1
        if num >= best_num:
            best_num, best_col, best_end = num, x, y + 1
    return best_num, best_col, best_end


# ---------------------------------------------------------------------------
# 01 overall framework
# ---------------------------------------------------------------------------
fig, ax = plt.subplots(figsize=(15, 8.6))
ax.set_xlim(0, 15)
ax.set_ylim(0, 8.6)
ax.axis("off")
ax.text(7.5, 8.25, "循迹小车代码整体框架", ha="center", va="center",
        fontproperties=FONT, fontsize=20, color="#1f3864", weight="bold")

box(ax, 1.55, 6.9, 2.25, 1.0, "MT9V03X 摄像头\n188×120 灰度图", LIGHT_YELLOW, YELLOW)
box(ax, 4.20, 6.9, 2.30, 1.0, "CSI + DMA\n双缓冲采集", LIGHT_YELLOW, YELLOW)
box(ax, 6.85, 6.9, 2.25, 1.0, "mt9v03x_image\n当前帧指针", LIGHT_YELLOW, YELLOW)
box(ax, 9.65, 6.9, 2.45, 1.0, "Binary_Img()\nOTSU + 二值化", LIGHT_GREEN, GREEN)
box(ax, 12.55, 6.9, 2.55, 1.0, "videoData[100][152]\n0=黑  1=白", LIGHT_GREEN, GREEN)

for x1, x2 in [(2.68, 3.04), (5.36, 5.71), (7.98, 8.41), (10.88, 11.26)]:
    arrow(ax, x1, 6.9, x2, 6.9, BLUE)

box(ax, 1.95, 4.9, 2.35, 0.9, "Find_EndRow()\n最长白列 / 截止行", LIGHT_BLUE, BLUE, fs=10.5)
box(ax, 4.75, 4.9, 2.35, 0.9, "FindBorder()\n逐行找左右边界", LIGHT_BLUE, BLUE, fs=10.5)
box(ax, 7.55, 4.9, 2.35, 0.9, "Find_MidLine()\n中线 + 像素误差", LIGHT_BLUE, BLUE, fs=10.5)
box(ax, 10.35, 4.9, 2.55, 0.9, "FindControlRow()\n选择控制行", LIGHT_BLUE, BLUE, fs=10.5)

ax.plot([12.55, 12.55, 1.95, 1.95], [6.38, 5.95, 5.95, 5.36],
        color=BLUE, lw=1.5, zorder=1)
arrow(ax, 1.95, 5.55, 1.95, 5.36, BLUE)
for x1, x2 in [(3.13, 3.56), (5.93, 6.36), (8.73, 9.06)]:
    arrow(ax, x1, 4.9, x2, 4.9, BLUE)

# direction branch
box(ax, 1.95, 2.65, 2.45, 0.9, "Get_Direction_Error()\n方向误差", LIGHT_RED, RED, fs=10.5, ls="--")
box(ax, 4.85, 2.65, 2.25, 0.9, "舵机 PD/PID\n（当前未实现）", LIGHT_RED, RED, fs=10.5, ls="--")
box(ax, 7.60, 2.65, 2.25, 0.9, "ServoControlOut\n（没有被误差更新）", LIGHT_RED, RED, fs=10.0, ls="--")
box(ax, 10.45, 2.65, 2.15, 0.9, "舵机 PWM C30\n固定输出 1100", LIGHT_GREEN, GREEN, fs=10.5)
arrow(ax, 3.18, 2.65, 3.72, 2.65, RED, ls="--")
arrow(ax, 5.98, 2.65, 6.47, 2.65, RED, ls="--")
arrow(ax, 8.73, 2.65, 9.37, 2.65, RED, ls="--")
ax.plot([10.35, 10.35, 10.35], [4.45, 3.6, 3.11], color=RED, lw=1.4, ls="--")
arrow(ax, 10.35, 3.35, 10.35, 3.11, RED, ls="--")
ax.text(5.4, 1.95, "红色虚线 = 当前源码缺失或未接通的转向闭环",
        ha="center", va="center", fontproperties=FONT, fontsize=10.5, color=RED)

# speed branch
box(ax, 1.95, 1.05, 2.25, 0.85, "Speed_Correct()\n左右目标速度", LIGHT_BLUE, BLUE, fs=10.5)
box(ax, 4.75, 1.05, 2.45, 0.85, "PIT 2ms\nSpeedControl()", LIGHT_BLUE, BLUE, fs=10.5)
box(ax, 7.65, 1.05, 2.30, 0.85, "Set_Motor_duty()\n方向 + PWM", LIGHT_BLUE, BLUE, fs=10.5)
box(ax, 10.55, 1.05, 2.10, 0.85, "左右电机\n+ 编码器", LIGHT_GREEN, GREEN, fs=10.5)
arrow(ax, 3.08, 1.05, 3.52, 1.05, BLUE)
arrow(ax, 5.98, 1.05, 6.50, 1.05, BLUE)
arrow(ax, 8.80, 1.05, 9.50, 1.05, BLUE)
ax.plot([10.55, 13.55, 13.55, 4.75], [0.62, 0.62, 0.25, 0.25],
        color=GREEN, lw=1.5, zorder=1)
arrow(ax, 4.75, 0.25, 4.75, 0.62, GREEN)
ax.text(12.0, 0.46, "编码器反馈", ha="center", va="center",
        fontproperties=FONT, fontsize=9.5, color=GREEN)

save(fig, "01_overall_framework.png")


# ---------------------------------------------------------------------------
# 02 image crop and binarization
# ---------------------------------------------------------------------------
gray_img = make_track()
binary_full = (gray_img > 150).astype(np.uint8)
binary_crop = binary_full[0:100, 10:162]

fig, axes = plt.subplots(1, 2, figsize=(14.5, 6.5))
ax = axes[0]
ax.imshow(gray_img, cmap="gray", vmin=0, vmax=255)
ax.add_patch(Rectangle((10, 0), 152, 100, fill=False, ec=RED, lw=2.5))
ax.text(12, 8, "裁剪区域：列 10~161，行 0~99", color=RED,
        fontproperties=FONT, fontsize=10.5, va="top",
        bbox=dict(boxstyle="round,pad=0.25", fc="white", ec=RED, alpha=0.9))
ax.set_title("原始灰度图：188×120", fontproperties=FONT, fontsize=14)
ax.set_xlabel("列 col", fontproperties=FONT)
ax.set_ylabel("行 row（0=远处，99=近处）", fontproperties=FONT)
for lab in ax.get_xticklabels() + ax.get_yticklabels():
    lab.set_fontproperties(FONT)

ax = axes[1]
ax.imshow(binary_crop, cmap="gray", vmin=0, vmax=1)
ax.set_title("二值图：100×152", fontproperties=FONT, fontsize=14)
ax.set_xlabel("列 col", fontproperties=FONT)
ax.set_ylabel("行 row", fontproperties=FONT)
for lab in ax.get_xticklabels() + ax.get_yticklabels():
    lab.set_fontproperties(FONT)
ax.text(4, 8, "白色=1：赛道\n黑色=0：边界/背景", color="white",
        fontproperties=FONT, fontsize=11, va="top",
        bbox=dict(boxstyle="round,pad=0.3", fc="black", ec="none", alpha=0.55))

fig.text(0.505, 0.12, "OTSU 求阈值\n+ LightPoint 补偿\n裁剪 + 二值化",
         ha="center", va="center", fontproperties=FONT, fontsize=11.5,
         bbox=dict(boxstyle="round,pad=0.4", fc=LIGHT_GREEN, ec=GREEN, lw=1.5))
fig.suptitle("从摄像头灰度图到循迹二值图", fontproperties=FONT, fontsize=18, y=1.02)
plt.tight_layout()
save(fig, "02_image_binary.png")


# ---------------------------------------------------------------------------
# 03 end row and max column
# ---------------------------------------------------------------------------
bin_img = make_binary_track()
run_len, max_col, end_row = longest_white_column(bin_img)
h, w = bin_img.shape

fig, ax = plt.subplots(figsize=(12.5, 7.2))
ax.imshow(bin_img, cmap="gray", vmin=0, vmax=1, origin="upper")
ax.set_title("Find_EndRow()：找最长白列和截止行", fontproperties=FONT, fontsize=17)
ax.set_xlabel("列 col", fontproperties=FONT)
ax.set_ylabel("行 row", fontproperties=FONT)
for lab in ax.get_xticklabels() + ax.get_yticklabels():
    lab.set_fontproperties(FONT)

for x in [28, 55, max_col, 112, 132]:
    y = h - 1
    while y >= 0 and bin_img[y, x] == 1:
        y -= 1
    ax.annotate("", xy=(x, y + 1), xytext=(x, h - 1),
                arrowprops=dict(arrowstyle="->", color="#1f77b4", lw=1.2, alpha=0.8))
    ax.text(x + 1.2, h - 2, "计数", color="#1f77b4", fontproperties=FONT, fontsize=8.5)

ax.plot([max_col, max_col], [h - 1, end_row], color=RED, lw=3)
ax.plot([0, max_col], [end_row, end_row], color=RED, lw=2, ls="--")
ax.scatter([max_col], [end_row], s=60, color=RED, zorder=5)
ax.annotate("Maxcolum = %d\n最长白列" % max_col,
            xy=(max_col, h - 4), xytext=(max_col + 18, h - 15),
            fontproperties=FONT, fontsize=11, color=RED,
            arrowprops=dict(arrowstyle="->", color=RED, lw=1.4))
ax.annotate("EndRow = %d\n最长白列顶端" % end_row,
            xy=(max_col, end_row), xytext=(max_col + 18, end_row + 14),
            fontproperties=FONT, fontsize=11, color=RED,
            arrowprops=dict(arrowstyle="->", color=RED, lw=1.4))

ax.axhline(95, color="#e69138", lw=1.8, ls="--")
ax.text(2, 92.5, "EndRow > 95 时 StopFlag = 1", color="#b45f06",
        fontproperties=FONT, fontsize=10.5)
ax.text(2, 3, "每一列都从最下方往上数连续白点；\n白点最多的一列决定 Maxcolum 和 EndRow。",
        color="white", fontproperties=FONT, fontsize=10.5, va="top",
        bbox=dict(boxstyle="round,pad=0.35", fc="black", ec="none", alpha=0.55))
save(fig, "03_endrow_maxcol.png")


# ---------------------------------------------------------------------------
# 04 border and midline
# ---------------------------------------------------------------------------
fig = plt.figure(figsize=(13.5, 8.2))
gs = GridSpec(2, 1, height_ratios=[2.4, 1.0], hspace=0.32)
ax = fig.add_subplot(gs[0])
ax.imshow(bin_img, cmap="gray", vmin=0, vmax=1, origin="upper")
ax.set_title("FindBorder() + Find_MidLine()：逐行找边界并算中线",
             fontproperties=FONT, fontsize=17)
ax.set_xlabel("列 col", fontproperties=FONT)
ax.set_ylabel("行 row", fontproperties=FONT)
for lab in ax.get_xticklabels() + ax.get_yticklabels():
    lab.set_fontproperties(FONT)

rows = np.arange(25, h)
lefts = []
rights = []
mids = []
for y in rows:
    xs = np.where(bin_img[y] == 1)[0]
    if len(xs) > 0:
        lefts.append(xs[0])
        rights.append(xs[-1])
        mids.append((xs[0] + xs[-1]) / 2.0)
ax.plot(lefts, rows, color="#e06666", lw=2.2, label="LeftLine（左边界内沿）")
ax.plot(rights, rows, color="#6fa8dc", lw=2.2, label="RightLine（右边界内沿）")
ax.plot(mids, rows, color="#ffd966", lw=2.2, label="MidLine（中线）")
ax.axvline(76, color="#93c47d", lw=1.6, ls="--", label="图像中心 col=76")
ax.legend(loc="lower right", prop=FONT, fontsize=9)

ax2 = fig.add_subplot(gs[1])
row = 72
row_pixels = bin_img[row:row + 1, :]
ax2.imshow(row_pixels, cmap="gray", vmin=0, vmax=1, aspect="auto",
           extent=[0, w, 0, 1], origin="upper")
ax2.set_ylim(-1.9, 2.0)
ax2.set_yticks([])
ax2.set_xlabel("列 col", fontproperties=FONT)
ax2.set_title("放大其中一行：黑色边界 → 白色赛道 → 黑色边界",
              fontproperties=FONT, fontsize=13)
for lab in ax2.get_xticklabels():
    lab.set_fontproperties(FONT)

xs = np.where(bin_img[row] == 1)[0]
left = int(xs[0])
right = int(xs[-1])
mid = (left + right) / 2.0
ax2.axvline(left, color="#e06666", lw=2)
ax2.axvline(right, color="#6fa8dc", lw=2)
ax2.axvline(mid, color="#ffd966", lw=2)
ax2.axvline(76, color="#93c47d", lw=1.5, ls="--")
ax2.annotate("LeftLine=%d" % left, xy=(left, 0.5), xytext=(left - 18, 1.45),
             fontproperties=FONT, fontsize=10, color="#cc0000",
             arrowprops=dict(arrowstyle="->", color="#cc0000"))
ax2.annotate("RightLine=%d" % right, xy=(right, 0.5), xytext=(right - 8, 1.45),
             fontproperties=FONT, fontsize=10, color="#3d85c6",
             arrowprops=dict(arrowstyle="->", color="#3d85c6"))
ax2.annotate("MidLine=%.1f" % mid, xy=(mid, 0.9), xytext=(mid, 1.65),
             ha="center", fontproperties=FONT, fontsize=10, color="#b45f06",
             arrowprops=dict(arrowstyle="->", color="#b45f06"))
ax2.text(76, -1.35, "Error = MidLine - 76 = %.1f" % (mid - 76),
         ha="center", va="center",
         fontproperties=FONT, fontsize=11,
         bbox=dict(boxstyle="round,pad=0.3", fc=LIGHT_YELLOW, ec=YELLOW, lw=1.2))
save(fig, "04_border_midline.png")


# ---------------------------------------------------------------------------
# 05 control loops
# ---------------------------------------------------------------------------
fig, ax = plt.subplots(figsize=(14.5, 8.0))
ax.set_xlim(0, 14.5)
ax.set_ylim(0, 8.0)
ax.axis("off")
ax.text(7.25, 7.65, "循迹控制的两个闭环", ha="center", va="center",
        fontproperties=FONT, fontsize=19, color="#1f3864", weight="bold")

ax.text(2.1, 7.15, "视觉外环：按摄像头帧率", ha="left", va="center",
        fontproperties=FONT, fontsize=11, color=BLUE)
box(ax, 2.1, 6.55, 2.25, 0.8, "摄像头一帧", LIGHT_YELLOW, YELLOW)
box(ax, 4.7, 6.55, 2.35, 0.8, "二值化 / 找边界\n算中线", LIGHT_BLUE, BLUE)
box(ax, 7.35, 6.55, 2.25, 0.8, "选择控制行\n得到误差", LIGHT_BLUE, BLUE)
arrow(ax, 3.23, 6.55, 3.53, 6.55, BLUE)
arrow(ax, 5.88, 6.55, 6.23, 6.55, BLUE)

box(ax, 2.2, 4.75, 2.55, 0.85, "Get_Direction_Error()\ndirectionError", LIGHT_RED, RED, ls="--")
box(ax, 5.1, 4.75, 2.45, 0.85, "舵机 PD/PID\n（缺失）", LIGHT_RED, RED, ls="--")
box(ax, 8.05, 4.75, 2.55, 0.85, "ServoControlOut\n→ 舵机 PWM", LIGHT_RED, RED, ls="--")
arrow(ax, 7.35, 6.14, 5.1, 5.18, RED, ls="--")
arrow(ax, 3.48, 4.75, 3.87, 4.75, RED, ls="--")
arrow(ax, 6.33, 4.75, 6.77, 4.75, RED, ls="--")
ax.text(5.1, 3.92, "当前源码没有接通这里：\n中线误差没有真正改变舵机输出",
        ha="center", va="center", fontproperties=FONT, fontsize=10.5, color=RED)

box(ax, 2.2, 2.95, 2.55, 0.85, "Speed_Correct()\n左右目标速度", LIGHT_BLUE, BLUE)
box(ax, 5.1, 2.95, 2.45, 0.85, "PIT 2ms\n500Hz", LIGHT_GREEN, GREEN)
box(ax, 8.05, 2.95, 2.55, 0.85, "增量式 PI\nPWM 输出", LIGHT_BLUE, BLUE)
box(ax, 11.15, 2.95, 2.55, 0.85, "左右电机\n+ 编码器", LIGHT_GREEN, GREEN)
arrow(ax, 3.48, 2.95, 3.87, 2.95, BLUE)
arrow(ax, 6.33, 2.95, 6.77, 2.95, BLUE)
arrow(ax, 9.33, 2.95, 9.87, 2.95, BLUE)
ax.plot([11.15, 13.7, 13.7, 5.1], [2.52, 2.52, 1.95, 1.95],
        color=GREEN, lw=1.6)
arrow(ax, 5.1, 1.95, 5.1, 2.52, GREEN)
ax.text(10.8, 2.1, "编码器测速反馈", fontproperties=FONT, fontsize=9.5, color=GREEN)

ax.text(2.2, 3.55, "速度内环：500Hz", ha="left", va="center",
        fontproperties=FONT, fontsize=11, color=GREEN)
ax.text(7.25, 1.25, "外环负责“往哪拐”，内环负责“轮子转多快”。\n当前版本外环的转向输出没有接通。",
        ha="center", va="center", fontproperties=FONT, fontsize=12,
        bbox=dict(boxstyle="round,pad=0.45", fc="#f3f3f3", ec="#999999", lw=1.2))
save(fig, "05_control_loops.png")


# ---------------------------------------------------------------------------
# 06 control row selection
# ---------------------------------------------------------------------------
fig, axes = plt.subplots(1, 2, figsize=(12.5, 7.0))
for ax in axes:
    ax.set_xlim(0, 1)
    ax.set_ylim(100, 0)
    ax.set_xlabel("图像列方向（示意）", fontproperties=FONT)
    ax.set_ylabel("行 row（0=远，99=近）", fontproperties=FONT)
    ax.set_xticks([])
    for lab in ax.get_yticks():
        pass

ax = axes[0]
ax.set_title("正常情况：EndRow < 55", fontproperties=FONT, fontsize=14)
ax.add_patch(Rectangle((0.25, 30), 0.5, 69, fc="white", ec="black", lw=1.5))
ax.axhline(30, color=RED, lw=2)
ax.axhline(55, color=BLUE, lw=2)
ax.text(0.03, 30, "EndRow=30", color=RED, fontproperties=FONT, fontsize=10, va="bottom")
ax.text(0.03, 55, "RowSet=55\nControlRow=55", color=BLUE, fontproperties=FONT, fontsize=10, va="bottom")
ax.annotate("控制行落在赛道内", xy=(0.5, 55), xytext=(0.57, 42),
            fontproperties=FONT, fontsize=11, color=BLUE,
            arrowprops=dict(arrowstyle="->", color=BLUE, lw=1.3))
ax.text(0.5, 88, "白色区域=赛道", ha="center", fontproperties=FONT, fontsize=10)

ax = axes[1]
ax.set_title("靠近/出界情况：EndRow > 55", fontproperties=FONT, fontsize=14)
ax.add_patch(Rectangle((0.25, 80), 0.5, 19, fc="white", ec="black", lw=1.5))
ax.axhline(80, color=RED, lw=2)
ax.axhline(55, color=BLUE, lw=2)
ax.axhline(81, color="#e69138", lw=2, ls="--")
ax.text(0.03, 80, "EndRow=80", color=RED, fontproperties=FONT, fontsize=10, va="bottom")
ax.text(0.03, 55, "RowSet=55", color=BLUE, fontproperties=FONT, fontsize=10, va="bottom")
ax.annotate("EndRow+1=81", xy=(0.78, 81), xytext=(0.54, 88),
            color="#b45f06", fontproperties=FONT, fontsize=10,
            arrowprops=dict(arrowstyle="->", color="#b45f06", lw=1.2))
ax.annotate("", xy=(0.78, 81), xytext=(0.78, 55),
            arrowprops=dict(arrowstyle="<->", color="#b45f06", lw=1.8, linestyle="--"))
ax.text(0.82, 68, "ControlRow\n在两行之间\n来回切换", color="#b45f06",
        fontproperties=FONT, fontsize=10.5, va="center")
ax.text(0.5, 95, "白色区域只有下方一小段", ha="center", fontproperties=FONT, fontsize=10)

fig.suptitle("FindControlRow() 的控制行选择示意", fontproperties=FONT, fontsize=18, y=1.02)
plt.tight_layout()
save(fig, "06_control_row.png")

print("all diagrams generated")



