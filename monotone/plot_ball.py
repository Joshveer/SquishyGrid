import matplotlib.pyplot as plt
import numpy as np
import re

x_vals = []
y_vals = []
angles=[]
lens=[]

with open("output_angle_len.txt", "r") as file:
    content = file.read()

# 使用正则表达式提取所有形如 (x,y) 的模式（支持换行和空格）
matches = re.findall(r'\(\s*([\d\.\-eE]+)\s*,\s*([\d\.\-eE]+)\s*\)', content)

for x_str, y_str in matches:
    angles.append(float(x_str))
    lens.append(float(y_str))
pairs=list(zip(angles,lens))
pairs.sort(key=lambda x: x[0])
angles[:],lens[:]=zip(*pairs)
plt.figure(figsize=(6,6))
plt.plot(angles,lens,color='red');
plt.xlabel("angle")
plt.ylabel("time constant")
plt.ylim(0.6,1.4)
#plt.title("Angle and Distance from Origin to Each Point")
plt.grid(True)
plt.savefig('angle.png')

plt.figure(figsize=(6, 6))

with open("output_ball.txt", "r") as file:
    content = file.read()

# 使用正则表达式提取所有形如 (x,y) 的模式（支持换行和空格）
matches = re.findall(r'\(\s*([\d\.\-eE]+)\s*,\s*([\d\.\-eE]+)\s*\)', content)

for x_str, y_str in matches:
    x_vals.append(float(x_str))
    y_vals.append(float(y_str))

pairs=list(zip(x_vals,y_vals));
pairs.sort(key=lambda x:np.atan2(x[0],x[1]));
x_vals[:],y_vals[:]=zip(*pairs)
plt.plot(x_vals, y_vals, color='red')

# 设置坐标轴标签和图标题
plt.xlabel('X')
plt.ylabel('Y')
plt.title('Ball')

# 设置网格
#plt.grid(True)

# 设置坐标轴等比例
#plt.axis('equal')
theta = np.linspace(np.pi/2, 0, 100)
#print(theta)
# 圆的参数方程：x = cos(θ), y = sin(θ)，半径为 1
x = np.cos(theta)
y = np.sin(theta)

# 绘图
#plt.figure(figsize=(5,5))
#print(x)
#print(y)
plt.plot(x, y,color='blue');
#plt.gca().set_aspect('equal')  # 坐标比例相同
plt.grid(True)
# 显示图例和图形
#plt.legend()
plt.savefig('ball.png');
