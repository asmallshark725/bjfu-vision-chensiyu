# RoboMaster C++ 学习

为 RoboMaster 视觉算法方向打基础，记录每周的 C++ 练习。

## 目录结构

仓库位置：`/home/shark/projects/bjfu-vision-chensiyu`。

- `week1/`：第一周源码及本地编译的可执行文件。
- `week2/`：第二周练习，目前包含 `day8.cpp`；后续 `day9.cpp` 也放在此目录。
- `task1/`：Ubuntu 与 Git 练习，说明见 [task1/README.md](task1/README.md)。

## 已练习内容

- 变量、输入输出与算术运算
- 条件判断与循环
- 质数判断与猜数字游戏
- 函数、返回值、引用参数与默认参数
- Git 基础操作

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

每个 .cpp 文件独立编译运行；可执行文件放在对应周目录，并由根目录 .gitignore 忽略。
第一周结业自测：已完成。
