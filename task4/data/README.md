# Task4 课程素材

使用同济 SP 26 培训仓库的第四讲课程样例。素材是室内手持红色 2 号装甲板，视频及参数取自同一固定版本。

- 仓库：<https://github.com/TongjiSuperPower/sp_vision_tutorial_26>
- 版本：`2bb6179ef9900bd1e5e815c287994d7357e62fe4`
- 原文件：`lecture4/class/video.avi`
- 分辨率：1280×1024；帧率：30 FPS；帧数：752。
- SHA-256：`70d04ce4a73197ee1d5e0e771c148f8f883866bc8a3a9dee86125dffde06533b`

在培训仓库根目录下载并检查：

```bash
curl --fail --location "https://raw.githubusercontent.com/TongjiSuperPower/sp_vision_tutorial_26/2bb6179ef9900bd1e5e815c287994d7357e62fe4/lecture4/class/video.avi" --output task4/data/video.avi
sha256sum task4/data/video.avi
./task4/build/video_read task4/data/video.avi
```

本机已有同一文件：Windows 为 `E:\robomaster\school_training\hello_armor\video.avi`，WSL 为 `/mnt/e/robomaster/school_training/hello_armor/video.avi`，可直接作为运行参数传入，无需重复下载。

视频由 `.gitignore` 排除。此目录提交来源记录和原仓库许可证；参数来自同一示例的 `lecture4/class/src/main.cpp`，已写入当前源码中。
