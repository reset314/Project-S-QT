import matplotlib.pyplot as plt
import numpy as np

# 创建画布
fig, ax = plt.subplots(figsize=(8, 6))

# 1. 绘制矩形（填充 #abc5b8）
rect = plt.Rectangle((-2, -1), 4, 4, facecolor='#abc5b8', edgecolor='none', zorder=1)
ax.add_patch(rect)

# 2. 计算切点与优弧
r = 1
x0 = 2 * np.sqrt(2) / 3
y0 = 1 / 3
theta1 = np.arctan2(y0, x0)          # 右切点角度
theta2 = np.arctan2(y0, -x0)         # 左切点角度

# 生成优弧上的点（经过圆下方）
N = 200
theta = np.linspace(theta2, theta1 + 2*np.pi, N)  # 逆时针从左侧切点绕到右侧切点（经过底部）
arc_x = r * np.cos(theta)
arc_y = r * np.sin(theta)

# 构建封闭路径：弧 + (0,3) + 回到弧起点（即右切点？注意弧的起点现在是theta2对应的点（左切点）？）
# 我们生成的弧从theta2（左切点）到theta1+2π（相当于右切点），所以起点是左切点，终点是右切点。
# 但我们要构成封闭图形，需要连接(0,3)和弧的两个端点。
# 通常做法：弧点列表从起点到终点，然后连接终点->(0,3)->起点，形成封闭。
# 这里起点是theta2对应的点（左切点），终点是theta1+2π对应的点（右切点）。
points_x = list(arc_x) + [0, arc_x[0]]  # 弧终点是arc_x[-1]？不，我们连接终点(arc_x[-1], arc_y[-1])到(0,3)，再从(0,3)连回起点(arc_x[0], arc_y[0])
points_y = list(arc_y) + [3, arc_y[0]]

# 3. 绘制曲边三角形（填充 #a3c1d4），置于上层（zorder=2）
ax.fill(points_x, points_y, facecolor='#a3c1d4', edgecolor='none', zorder=2)

# （可选）显示圆和切线辅助线，取消注释即可
# circle = plt.Circle((0, 0), 1, fill=False, edgecolor='gray', linestyle='dashed', linewidth=0.5)
# ax.add_patch(circle)
# ax.plot([x0, 0], [y0, 3], color='black', linewidth=0.5)
# ax.plot([-x0, 0], [y0, 3], color='black', linewidth=0.5)

# 4. 设置坐标轴范围（保留足够空间显示图形）
ax.set_xlim(-3, 3)
ax.set_ylim(-2, 4)
ax.set_aspect('equal')

# 5. 完全隐藏坐标系
ax.set_axis_off()

# 6. 导出矢量图（SVG 和 PDF）
plt.savefig('curve_triangle_rectangle_优弧.svg', format='svg', bbox_inches='tight')
plt.savefig('curve_triangle_rectangle_优弧.pdf', format='pdf', bbox_inches='tight')

# 如需预览，取消下方注释
# plt.show()