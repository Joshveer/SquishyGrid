import matplotlib.pyplot as plt
import numpy as np
import re

x_vals = []
y_vals = []

with open("output_ball(4,30000,1000).txt", "r") as file:
    content = file.read()

# 使用正则表达式提取所有形如 (x,y) 的模式（支持换行和空格）
matches = re.findall(r'\(\s*([\d\.\-eE]+)\s*,\s*([\d\.\-eE]+)\s*\)', content)

for x_str, y_str in matches:
    x_vals.append(float(x_str))
    y_vals.append(float(y_str))
plt.figure(figsize=(6,6))
x=np.array(x_vals);
y=np.array(y_vals);
angles=np.arctan2(y,x)
lengths=np.hypot(x,y);
plt.plot(angles,lengths,color='red');
plt.xlabel("angle")
plt.ylabel("time constant")
plt.ylim(0.8,1.2)
#plt.title("Angle and Distance from Origin to Each Point")
plt.grid(True)
plt.savefig('angle.png')
plt.figure(figsize=(6, 6))
for i in range(len(x_vals)):
    l=x_vals[i]**2+y_vals[i]**2
    x_vals[i]=x_vals[i]/l
    y_vals[i]=y_vals[i]/l

plt.plot(x_vals, y_vals, color='red')

# 设置坐标轴标签和图标题
plt.xlabel('X')
plt.ylabel('Y')
plt.title('ball')

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
plt.savefig('1.png');
# 添加坐标轴
#plt.axhline(0, color='red', linewidth=0.5)
#plt.axvline(0, color='red', linewidth=0.5)

#plt.gca().set_aspect('equal')  # 坐标比例相同
plt.grid(True)
# 显示图例和图形
#plt.legend()
plt.savefig('ball.png');
