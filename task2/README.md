# 任务二：Hello C++

本目录记录第 2—3 周的 C++ 作业：Hello World、成绩统计、三种交换方式，以及多文件组织、CMake 构建、文本文件输入和 VSCode 断点调试。

当前成绩程序的目标名是 `score_app`，由 `main.cpp` 和 `src/scores.cpp` 共同构建，从 `scores.txt` 读取数据。第 2 周的三个单文件练习保留，仍可分别编译运行。

## 开发环境

- Windows 11 + WSL2 Ubuntu 22.04
- VSCode + Remote-WSL
- g++ 11.4.0，使用 C++17
- CMake 3.22.1，使用 Make 构建
- GDB 12.1，用于 VSCode 调试

## 快速开始：多文件成绩程序

已准备好 g++、CMake 和 Make 的 Ubuntu 环境可执行以下命令：

```bash
git clone https://github.com/asmallshark725/bjfu-vision-chensiyu.git
cd bjfu-vision-chensiyu/task2
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/score_app
```

如果已经克隆仓库，从仓库根目录进入 `task2`，再执行上述配置、构建和运行命令即可。

必须在 `task2` 目录运行 `./build/score_app`。代码中的相对路径 `scores.txt` 按程序运行时的工作目录查找；可执行程序位于 `build`，不会自动将工作目录切换到 `build`。

### 数据文件与预期输出

仓库中的 `scores.txt` 内容为：

```text
5
65 95 78 32 52
```

第一个整数是分数数量 `N`，后面是 `N` 个整数分数。分隔符可以是空格或换行。

运行结果为：

```text
95 78 65 52 32
max:95
min:32
sum:322
ar:64.40
```

其中 `max`、`min`、`sum`、`ar` 分别表示最高分、最低分、总分和平均分。平均分保留两位小数。

## 文件说明

| 文件 | 用途 |
| --- | --- |
| `main.cpp` | 文件读取、输入检查、调用求和函数、排序和输出统计结果 |
| `include/scores.hpp` | 声明 `Sumscore` 求和函数 |
| `src/scores.cpp` | 定义 `Sumscore`，遍历成绩并返回总分 |
| `CMakeLists.txt` | 设置 C++17，组织两个源文件和头文件搜索目录，生成 `score_app` |
| `scores.txt` | 多文件成绩程序的输入数据 |
| `hello.cpp` | 输出欢迎文字，验证 C++ 程序的编译与运行 |
| `scores.cpp` | 第 2 周的键盘输入版成绩统计练习，独立编译运行 |
| `swap.cpp` | 对比传值、传指针、传引用三种交换方式 |
| `images/` | 成绩统计、交换实验、CMake 构建和断点调试截图 |

`src/scores.cpp` 提供求和函数实现；同目录外的 `scores.cpp` 是包含独立 `main` 的旧练习。CMake 的 `score_app` 目标只使用 `main.cpp` 和 `src/scores.cpp`。

## CMake 构建说明

`add_executable(score_app main.cpp src/scores.cpp)` 创建一个名为 `score_app` 的可执行目标，并列出参与编译链接的两个源文件。

`target_include_directories(score_app PRIVATE include)` 为这个目标添加头文件搜索目录，因此两个源文件都可以使用 `#include "scores.hpp"`。

- 配置命令 `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug` 读取当前目录的 `CMakeLists.txt`，在 `build` 中生成构建文件，并启用调试信息。
- 构建命令 `cmake --build build` 调用配置好的构建工具，完成编译和链接。在当前 Make 环境中，这与进入构建目录后执行 `make` 对应。
- 修改 C++ 代码后需要重新构建，再运行 `./build/score_app`；仅修改 `scores.txt` 时，直接重新运行即可。
- 仓库根目录的 `.gitignore` 已排除 `/task2/build/`；构建产物可以重新生成。

## 第 2 周单文件练习：编译与运行

以下命令在 Ubuntu 终端中执行。先从仓库根目录进入本任务目录：

```bash
cd task2
```

后面三个程序的命令都在 `task2` 目录中执行。

### 1. Hello World

```bash
g++ -std=c++17 -o hello hello.cpp
./hello
```

程序输出欢迎文字，用于验证编译运行流程。

### 2. 成绩统计

```bash
g++ -std=c++17 -o scores scores.cpp
./scores
```

程序启动后，先输入分数数量 `N`，再输入 `N` 个整数分数。分数之间可以用空格或换行分隔。

示例输入：

```text
5
65 98 45 21 53
```

对应输出：

```text
98 65 53 45 21
max:98
min:21
sum:282
ar:56.40
```

其中 `max` 表示最高分，`min` 表示最低分，`sum` 表示总分，`ar` 表示平均分。平均分保留两位小数。`N <= 0` 时，程序给出提示并结束。

### 3. 三种交换方式

```bash
g++ -std=c++17 -o swap swap.cpp
./swap
```

该程序不需要键盘输入。每次交换实验前，将 `x`、`y` 恢复为 `3`、`7`，调用后的结果如下：

| 传参方式 | 函数调用结束后，`main` 中的 `x`、`y` |
| --- | --- |
| 传值 | `3 7` |
| 传指针 | `7 3` |
| 传引用 | `7 3` |

程序还保留了一个独立的指针实验：通过 `*p = 10` 将 `x` 修改为 `10`，验证通过指针访问并修改原变量。

## 关键实现说明

### 声明、定义与调用

`include/scores.hpp` 通过 `#pragma once` 防止同一编译单元内重复包含，并声明：

```cpp
int Sumscore(const std::vector<int>& scores);
```

`src/scores.cpp` 包含这个头文件，提供求和函数的完整定义。`main.cpp` 也包含这个头文件，并用 `int sum = Sumscore(scores);` 接收返回值。两个 `.cpp` 分别编译，随后由链接器将函数调用与实现连接起来。

参数中的 `&` 表示引用，避免复制整个 vector；`const` 表示函数通过这个参数只读取成绩，不修改成绩。

### 文件读取与错误处理

- 用 `std::ifstream fin("scores.txt")` 打开输入文件，用 `fin.is_open()` 检查是否打开成功。
- 用 `if (!(fin >> n) || n <= 0)` 先尝试读取数量，再检查数量是否为正数。`||` 从左到右判断，读取失败时会直接进入错误分支。
- 每轮用 `if (!(fin >> score))` 读取并检查一个成绩；读取成功后才调用 `push_back`。条件中的 `fin >> score` 本身会读取数据，每轮只执行一次读取。
- 同一个 `fin` 会记住读取位置，后续提取会继续读取下一个整数。
- 正常完成时 `main` 返回 `0`；打开或读取失败时返回 `1`，并输出错误提示。

| 输入情况 | 程序行为 |
| --- | --- |
| 数据数量正确且可以读成整数 | 输出降序成绩及统计结果，退出码为 `0` |
| 工作目录中没有 `scores.txt` | 输出“无法打开 scores.txt”，退出码为 `1` |
| 数量读取失败，或 `N <= 0` | 输出“成绩不足或格式错误”，退出码为 `1` |
| 后续不足 `N` 个成绩，或某个成绩读取失败 | 输出“成绩不足或格式错误”，退出码为 `1` |

### 成绩统计

- 用 `std::vector<int>` 保存输入的分数，用循环累加总分。
- 用 `sum * 1.0 / n` 计算平均值，使除法按浮点数运算，保留小数部分。
- 用 `std::sort` 配合 `std::greater<int>()` 将分数按从高到低排序。
- 降序排列后，第一个元素是最大值，最后一个元素是最小值。

### 为什么传值交换不了原变量

传值时，函数参数得到原变量值的副本。函数内部交换的是副本，因此函数内部可以输出 `7 3`，但返回 `main` 后，原来的 `x`、`y` 仍为 `3 7`。

### 指针与引用

- 传指针：函数接收原变量的地址，通过 `*` 访问并修改原变量，因此能交换原变量的值。
- 传引用：参数是原变量的别名，通过引用操作的就是原变量，因此也能交换原变量的值。

## 运行效果

### 多文件成绩程序：CMake 构建与文件输入

截图中可以看到 `src/scores.cpp.o` 的编译、`score_app` 的链接，以及总分 `322`、平均分 `64.40` 的运行结果。

![多文件成绩程序的 CMake 构建和文件输入结果](images/cmake_file_input.png)

### 第 2 周键盘输入版成绩统计

![成绩统计运行结果](images/scores.png)

### 三种交换方式

![三种交换方式运行结果](images/swap.png)

## VSCode 断点与单步调试记录

以下截图来自 `week3/day4.cpp` 文件读取练习，记录了使用 GDB 打断点、按 F10 单步执行和观察变量变化的过程。该练习与 `task2` 成绩程序使用相同的文件读取与检查方式。

### 1. 停在读取数量之前

断点设在 `if (!(fin >> n) || n <= 0)` 所在行。程序暂停时，黄色高亮行尚未执行，此时初始化后的 `n` 为 `0`。

![读取数量前的断点，n 为 0](images/debug_before_read.png)

### 2. 单步执行读取语句

按 F10 执行这一行后，程序从文件读到数量 `5`，`n` 变为 `5`，并继续走正常分支。

![单步执行读取后，n 变为 5](images/debug_after_read.png)

### 3. 观察循环中的变量

在 `scores.push_back(score)` 处设置断点，按 F5 继续。截图中 `i = 2`、`score = 78`，表示第三轮循环已读到第三个成绩，尚未执行本轮的追加操作。循环每次经过断点都会暂停，按 F5 会继续本次运行。

![循环中的断点，i 为 2，score 为 78](images/debug_loop.png)

### 调试当前的 score_app

先按“快速开始”中的 Debug 配置命令构建。在仓库根目录的 `.vscode/launch.json` 中使用 C++/GDB 调试配置，程序路径设为 `${workspaceFolder}/task2/build/score_app`，工作目录设为 `${workspaceFolder}/task2`，调试器路径使用 `/usr/bin/gdb`。

在 `main.cpp` 的求和调用处设置断点，F5 启动后可用 F10 执行当前行，或用 F11 进入 `src/scores.cpp` 中的求和函数。修改源码后，先重新构建，再启动调试。
