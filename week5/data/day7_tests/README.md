# Day7 测试素材

三张图片来自 [rzhu98-hub/RM_AutoAim 的 selected_armor_img 目录](https://github.com/rzhu98-hub/RM_AutoAim/tree/main/armor_detection_py/selected_armor_img)。原始地址、下载日期和 SHA-256 记录在 `sources.json` 中。图片保持下载原样，三个文件的哈希已核对。

| 文件 | 观察重点 |
| --- | --- |
| `01_blue3.jpg` | 较小的蓝色 3 号目标，以及其他侧面灯光 |
| `02_multi.jpg` | 红蓝目标同框、跨目标误配及亮斑干扰 |
| `03_dark.jpg` | 暗场蓝色目标、长条灯光及亮斑干扰 |

当前程序默认读取 `02_multi.jpg`。从 `week5` 目录运行，图片路径为 `data/day7_tests/02_multi.jpg`；更换素材需要修改 `src/day7.cpp` 中的读取路径并重新编译。

现有流程先提取亮中心，再对灯条附近进行红蓝采样，并按终端输入的 `Red` 或 `Blue` 筛选。当前阈值、窗口操作和验证结果见 [Day7 运行说明](../../docs/day7.md)。

三张图已用于调参与观察。最近对 `02_multi.jpg` 做离线检查，亮度 200、色差 50 时，选择 `Blue` 或 `Red` 分别得到 1 个装甲板候选、2 根显示灯条。该结果只覆盖这张图和这组参数；这些素材来自同一公开仓库，没有完整人工标注，不能据此计算跨场景准确率。

继续测试时记录图片名、目标颜色、亮度与色差阈值，以及漏检或误配的位置。未形成目标灯条对时，先查看亮度分割、单灯条筛选及颜色采样输出，再判断是否需要调整配对条件。程序会覆盖 `results/day7/Day7_contours.png` 和 `results/day7/Day7_mask.png`，需要比较多次结果时，先另存输出。
