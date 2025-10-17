#! /bin/bash

# 指定ndk路径
NDK=/data/user/0/com.termux/files/home/android-ndk-r21b/ndk-build

# 设置当前环境路径
path=$(pwd)
cd $path

# 清除缓存
if [[ -d $path/obj ]]
then
    cd $path/obj
    rm -rf $path/obj/local
    echo "清除缓存"
    cd $path
fi

# 开始编译
if [[ -f $NDK ]]
then
    $NDK
else
    echo "NDK不存在/异常"
fi

# 重命名lib5.so.fix
if [[ -f $path/libs/arm64-v8a/lib5.so.fix.so ]]
then
    mv $path/libs/arm64-v8a/lib5.so.fix.so $path/libs/arm64-v8a/lib5.so.fix
fi

echo "编译成功"