# 装甲板公开图片检查素材

新增 58 张原图，来自 4 个公开项目，覆盖红蓝同框、近距离多车、远距离小目标、倾斜、遮挡、反光和暗场。原图与标签保持下载时的字节内容；文件名和来源见 `manifest.json`。

打开 [图片目录](index.html) 可按来源浏览、点击查看原图。每张图有 `armor_001` 这样的编号，方便记录检查结果；编号仅用于本素材包。

## 图片与标签

| 目录 | 图片数 | 标签 | 观察重点 |
| --- | ---: | --- | --- |
| `images/ansidd/` | 25 | `labels/ansidd/` 中的 YOLO 检测框 | 直播与实验室画面、远景多车、侧视、遮挡；直播画面保留原有比分界面 |
| `images/ynurmod/` | 12 | 无 | 不同距离、光照与拍摄视角 |
| `images/rzhu/` | 7 | 无 | 红蓝灯条、过曝、暗场；避开已有的三张素材 |
| `images/qdu_damo/` | 8 | `labels/qdu_damo/` 中的 JSON | 工业相机、蓝色目标旋转与透视 |
| `images/qdu_gimbal_0507/` | 4 | 同名目录中的 JSON | 暗场小目标及行人遮挡 |
| `images/qdu_gimbal_0509/` | 2 | 同名目录中的 JSON | 暗场多块红色装甲板、反光干扰 |

共 39 张图片带上游标签，19 张无标签。YOLO 标签只有类别 `0` 的装甲板检测框，没有红蓝或角点字段。青岛大学 JSON 包含颜色、角点或检测框，来源登记为自动标注，部分对象带 `role: ignore`；这些标签已保留，尚未逐目标人工验收。

图片已经过解码检查、SHA-256 去重及缩略图筛选；去掉了一批明显相似的连续帧。保留的同场景帧仍可能相关，当前素材用于学习、调参和功能检查，未划分独立训练集与测试集。

## 用当前 Day7 程序检查

当前程序读取固定路径。先在 `src/day7.cpp` 的 `cv::imread` 中选一张图，例如：

```cpp
cv::Mat image = cv::imread("data/armor_public/images/rzhu/198.jpg",
                           cv::IMREAD_COLOR);
```

从仓库根目录编译，再进入 `week5` 运行：

```bash
cmake --build week5/build --target day7
cd week5
./build/day7
```

可先按下面顺序选图：

- 红色正面：`images/rzhu/198.jpg`。
- 蓝色正面：`images/rzhu/282.jpg`、`images/rzhu/413.jpg`。
- 红蓝同框：`images/ynurmod/1.jpg`。
- 暗场：`images/rzhu/issue_red_001.jpg`。
- 工业相机与旋转：`images/qdu_damo/`。
- 小目标、遮挡与反光：两个 `qdu_gimbal` 目录。

记录图片编号、目标颜色、亮度、色差、候选装甲板数量，以及漏检或误配位置。比较同一张图片的红蓝选择时，保持亮度和色差参数一致。分辨率和灯条大小不同，需要结合轮廓输出判断面积或长宽比筛选是否造成漏检。

程序仍会覆盖 `results/day7/Day7_contours.png` 和 `results/day7/Day7_mask.png`，需要比较结果时先另存。此素材包没有改动检测程序，也没有运行批量识别或模型训练。

## 来源与使用范围

- [ansidd/RMPlateDetection](https://github.com/ansidd/RMPlateDetection)：原图与对应的 YOLO 检测框。
- [houhongyi/RM-DATASET](https://github.com/houhongyi/RM-DATASET)：`Label_V2/Picture_Data` 中的示例原图。
- [rzhu98-hub/RM_AutoAim](https://github.com/rzhu98-hub/RM_AutoAim)：`selected_armor_img` 中的原图。
- [rmcv/armor](https://huggingface.co/datasets/rmcv/armor)：青岛大学三个自采来源，来源表登记为 CC-BY-4.0。原始来源表和许可说明保存在 `provenance/`。

三个 GitHub 来源未找到图像数据的明确许可说明，清单中标为 `not_specified_for_images`。`sources.json` 记录来源与版本，`manifest.json` 逐文件记录下载地址、SHA-256、原始标签格式和序列分组。

后续训练时应先核对标签、统一格式，并按原始视频或采集序列划分数据。`source_batch` 只是上游文件名批次，不能视为独立拍摄序列；无法确认序列的同来源图片采用保守分组。
