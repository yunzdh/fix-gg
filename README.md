# fix_gg

## 介绍

由于gg修改器年久失修，使用process_vm_readv无法读取近些年游戏使用的内存保护方式（将内存段权限改为 -w-p)
,此模块可在读取失败时使用你的内核读取函数

## 使用方法

1. 完善vm_readv函数为你的内核读取函数
2. 编译此动态库重命名为lib5.so.fix
3. 重命名/data/user/0/com.study_tool.x64/files/GG-VH09/lib5.so为lib5.so.tmp
4. 新建文件/data/user/0/com.study_tool.x64/files/GG-VH09/lib5.so 里面写入

``` 
#!/bin/sh
export LD_PRELOAD="$0.fix"
exec "$0.tmp" "$@"
```
5. 将 lib5.so.fix 放入 /data/user/0/com.study_tool.x64/files/GG-VH09/