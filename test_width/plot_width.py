import matplotlib.pyplot as plt

import re

colors={'0':'blue','27':'red','45':'green'}
labels={'0':'0','27':r"$\frac{\pi}{8}$",'45':r"$\frac{\pi}{4}$"}
plt.figure(figsize=(6,6))
for name in {'0','27','45'}:

    x_vals = []
    y_vals = []

    with open("output_ball_"+name+"d.txt", "r") as file:
        content = file.read()
    
    # 使用正则表达式提取所有形如 (x,y) 的模式（支持换行和空格）
    matches = re.findall(r'\(\s*([\d\.\-eE]+)\s*,\s*([\d\.\-eE]+)\s*\)', content)

    for x_str, y_str in matches:
        x_vals.append(float(x_str))
        y_vals.append(float(y_str))
    plt.plot(x_vals, y_vals, color=colors[name], label=labels[name])
plt.xlabel('width')
plt.ylabel(r"$d(u)/||u||_2$")
plt.title('width')
plt.grid(True)
#plt.autoscale();
plt.ylim(0.9,1.1)
plt.savefig("width.png");
