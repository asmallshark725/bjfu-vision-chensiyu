# Week5 OpenCV 与装甲板识别练习

源码、输入素材、运行结果和说明分别存放。继续 Day7 时，主要查看 `src/day7.cpp`、`data/` 和 `results/day7/`。

## 目录

```text
week5/
├── CMakeLists.txt        构建入口
├── README.md             本说明
├── src/                  day1.cpp ～ day7.cpp
├── docs/                 Day6、Day7 的学习与运行说明
├── data/                 原图、测试素材与来源记录
│   ├── test.jpg
│   ├── day6_blue.jpg
│   ├── day6_source.txt
│   ├── day7_tests/        原有三张测试图
│   └── armor_public/     新收集的 58 张图片及标签
├── results/              按 day2 ～ day7 分类的结果图和掩膜
└── build/                编译产物，由 CMake 管理
```

旧的手工编译文件 `day1` 已移到 `build/legacy/`。

## 编译与运行

从仓库根目录执行：

```bash
cmake -S week5 -B week5/build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build week5/build --target day7
cd week5
./build/day7
```

运行其他练习时，把目标名 `day7` 换成对应的 `day1` ～ `day6`。执行 `cmake --build week5/build` 可以编译全部练习。

程序从 `week5` 目录运行，输入路径以 `data/` 开头，输出路径以 `results/dayN/` 开头。移动源码后，重新配置 CMake 也会更新 VS Code 使用的 `build/compile_commands.json`。

## 学习入口

- [Day1～Day5 源码](src/)：读图、像素处理、阈值、滑条和轮廓练习。
- [Day6 说明](docs/day6.md)：灯条几何筛选与 `Light` 对象。
- [Day7 说明](docs/day7.md)：灯条颜色、目标筛选、配对与评分输出。
- [原有三张测试图](data/day7_tests/README.md)。
- [新增素材说明](data/armor_public/README.md)与[图片浏览目录](data/armor_public/index.html)。
- [Day7 结果目录](results/day7/)：查看最新标注图和亮度掩膜。

## 选择测试图

在 `src/day7.cpp` 的 `cv::imread` 中修改输入路径，例如：

```cpp
cv::Mat image = cv::imread("data/armor_public/images/rzhu/198.jpg",
                           cv::IMREAD_COLOR);
```

修改后重新编译。终端输入 `Red` 或 `Blue`，通过滑条调参，按 Esc 执行检测。结果保存到 `results/day7/Day7_contours.png` 和 `results/day7/Day7_mask.png`，再次运行会覆盖这两张图。
