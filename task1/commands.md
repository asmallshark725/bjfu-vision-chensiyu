# 终端命令练习
| 命令 | 作用 |
|---|---|
| pwd | 显示当前目录的完整路径 |
| ls | 列出当前目录内容 |
| ls -l | 显示详细信息，包括类型、权限和大小 |
| cd 目录名 | 进入指定目录 |
| cd .. | 返回上一级目录 |
| mkdir archive | 创建 archive 目录 |
| mkdir -p task1/terminal_practice | 创建目录，缺少的上级目录也一起创建 |
| touch note.txt | 文件不存在时创建空文件，否则更新时间 |
| cp note.txt note_copy.txt | 复制文件，保留原文件 |
| mv note_copy.txt backup.txt | 重命名文件 |
| mv backup.txt archive/ | 将文件移入 archive 目录 |
| rm archive/backup.txt | 删除指定文件 |
| chmod u+x note.txt | 给所有者增加执行权限 |
| chmod u-x note.txt | 撤销所有者的执行权限 |

注意：ls -l 中是小写字母 l，不是数字 1。