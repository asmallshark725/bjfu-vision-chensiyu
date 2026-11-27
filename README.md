# RoboMaster C++ 学习

为 RoboMaster 视觉算法方向打基础，记录每周的 C++ 练习。

## 目录结构

仓库位置：`/home/shark/projects/bjfu-vision-chensiyu`。

- `week1/`：第一周源码及本地编译的可执行文件。
- `week2/`：第二周练习，目前包含 `day8.cpp`；后续 `day9.cpp` 也放在此目录。
- `task1/`：Ubuntu 与 Git 练习，说明见 [task1/README.md](task1/README.md)。
- `task2/`：C++ 多文件与 CMake 练习，说明见 [task2/README.md](task2/README.md)。
- `task3/`：Light、Armor、Detector 类与多态练习，说明见 [task3/README.md](task3/README.md)。
- `week5/`：OpenCV 图片处理、灯条筛选与红蓝配对，说明见 [week5/README.md](week5/README.md)。
- `week6/`：视频检测与 PnP 的逐步学习源码，说明见 [week6/README.md](week6/README.md)。
- `task4/`：本次任务提交入口，沿用现有架构完成检测、PnP、旋转颜色采样与耗时统计，见 [task4/README.md](task4/README.md)。

## 已练习内容

- 变量、输入输出与算术运算
- 条件判断与循环
- 质数判断与猜数字游戏
- 函数、返回值、引用参数与默认参数
- Git 基础操作
- OpenCV 图像、掩膜、轮廓、灯条几何筛选与配对
- 视频逐帧读取、阈值调节、旋转区域颜色采样和检测结果显示
- 四点 PnP、深度与距离、重投影偏差和单帧耗时统计

## 开发环境

Ubuntu 22.04（WSL）、g++、C++17。

## 编译与运行

以下命令均在仓库根目录执行。以第一周的 `week1/func1.cpp` 为例：

```bash
g++ -std=c++17 -o week1/func1 week1/func1.cpp
./week1/func1
```

第二周的编译与运行示例：

```bash
g++ -std=c++17 -o week2/day8 week2/day8.cpp
./week2/day8
```

早期单文件练习可独立编译；task2、task3、week5、week6 和 task4 按各自 README 使用 CMake 构建。构建目录由根目录 .gitignore 忽略。
第一周结业自测：已完成。
