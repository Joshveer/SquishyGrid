import matplotlib.pyplot as plt
import numpy as np
import re

x_vals = []
y_vals = []
angles=[]
lens=[]

plt.figure(figsize=(6, 6))

with open("output_ball.txt", "r") as file:
    content = file.read()

# 使用正则表达式提取所有形如 (x,y) 的模式（支持换行和空格）
matches = re.findall(r'\(\s*([\d\.\-eE]+)\s*,\s*([\d\.\-eE]+)\s*\)', content)

for x_str, y_str in matches:
    x_vals.append(float(x_str))
    y_vals.append(float(y_str))

p=x_vals.pop()
var=y_vals.pop()
pairs=list(zip(x_vals,y_vals));
pairs.sort(key=lambda x:np.atan2(x[0],x[1]));
x_vals[:],y_vals[:]=zip(*pairs)
plt.plot(x_vals, y_vals, color='red')
theta_vals=np.atan2(y_vals,x_vals)
norm_p_vals=(np.cos(theta_vals)**p+np.sin(theta_vals)**p)**(1/p)
norm_2_vals=(np.abs(x_vals)**2+np.abs(y_vals)**2)**(1/2)
#plt.plot(x_vals,1/norm_p_vals,'green')
#plt.plot(x_vals,norm_2_vals,'yellow')

max_err=np.max(np.abs(1/norm_p_vals-norm_2_vals))
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
x=np.cos(theta)
y=np.sin(theta)
norm = (np.abs(x)**p + np.abs(y)**p)**(1/p)
x/=norm
y/=norm
print("p:",p)
print("variance:",var)
print("max_err:",max_err)
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
