# Week6 视频练习

本目录保留逐步编写的视频检测源码。已经加入视频读取、亮度与色差掩膜、轮廓筛选、旋转区域颜色统计、灯条配对、四端点、PnP 测距、重投影和单帧计时。

Task4 的提交整理见 [task4/README.md](../task4/README.md)。它以这里的现有实现为基础，继续采用单文件流程并复用 Task3 的 `Light`、`Armor`，提供本次演示与素材说明。

以下命令在仓库根目录运行：

```bash
cmake -S week6 -B week6/build
cmake --build week6/build
./week6/build/video_read
```

本学习版本沿用 E 盘课程视频的本机路径，在终端输入 `Red` / `Blue`，按 `Esc` 退出。Task4 的整理版本另外支持命令行指定视频路径，便于在其他电脑复现。
