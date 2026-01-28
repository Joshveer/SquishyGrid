import matplotlib.pyplot as plt
import numpy as np
import re

x_vals = []
y_vals = []
angles=[]
lens=[]


plt.figure(figsize=(6, 6))

with open("output_path.txt", "r") as file:
    content = file.read()

# 使用正则表达式提取所有形如 (x,y) 的模式（支持换行和空格）
matches = re.findall(r'\(\s*([\d\.\-eE]+)\s*,\s*([\d\.\-eE]+)\s*\)', content)

for x_str, y_str in matches:
    x_vals.append(float(x_str))
    y_vals.append(float(y_str))

plt.plot(x_vals, y_vals, color='red')

# 设置坐标轴标签和图标题
plt.xlabel('X')
plt.ylabel('Y')
plt.title('Path')

# 显示图例和图形
#plt.legend()
plt.savefig('path.png');
