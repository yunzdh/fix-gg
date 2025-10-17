# kma_fix_gg

## 介绍

由于gg修改器年久失修，使用process_vm_readv无法读取近些年游戏使用的内存保护方式（将内存段权限改为 -w-p)，以及ptrace读写方式容易被检测
，此模块修改禁用process_vm_readv和ptrace，禁用selinux宽容模式，更改为更安全的读写方式即使用KMA驱动读写。

## 相关链接
KMA驱动 https://discord.gg/y6nzS7TmuM
fix_gg https://github.com/enenH/fix-gg

## 使用方法
1. 导入KMA相关的最新库driver.a、driver.h
2. 安装KMA相关的kpm模块
3. 编译此动态库重命名为lib5.so.fix
4. 重命名/data/user/0/[GG修改器包名]/files/GG-[随机名称]/lib5.so为lib5.so.tmp
5. 复制lib5.so到目录 /data/user/0/[GG修改器包名]/files/GG-[随机名称]/ 下，或者在该目录下新建lib5.so并在里面写入

``` 
#!/bin/sh
export LD_PRELOAD="$0.fix"
exec "$0.tmp" "$@"
```
6. 将 lib5.so.fix 放入 /data/user/0/[GG修改器包名]/files/GG-[随机名称]/
7. 将 lib5.so 和 lib5.so.fix 权限修正755，所有者和用户者修正为与lib5.so.tmp一致