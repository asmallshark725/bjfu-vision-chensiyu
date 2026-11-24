# Week6 视频练习

`video_read.cpp` 保留自主编写的初版视频灯条检测过程，继续使用原来的 CMake 目标 `video_read`。

学校任务四的正式整理、优化和测距实现放在 [task4/](../task4/README.md)：它复用任务三的 `Light`、`Armor`，补充模块拆分、倾斜配对、PnP、耗时统计及结果展示。后续阅读和运行以 task4 的说明为入口。

```bash
cmake -S week6 -B week6/build
cmake --build week6/build
./week6/build/video_read
```

上面的初版仍读取本机 E 盘课程视频路径，并在终端输入 `Red` / `Blue`；正式 task4 提供可替换视频和相机配置的参数。
