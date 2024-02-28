#!/bin/bash

set -e

# 获取当前工作目录
CURRENT_DIR=$(pwd)

# 如果没有build目录，创建该目录
if [ ! -d "$CURRENT_DIR/build" ]; then
    mkdir "$CURRENT_DIR/build"
fi

# 清空build目录
rm -rf "$CURRENT_DIR/build/*"

# 进入build目录
cd "$CURRENT_DIR/build" &&
    cmake .. &&
    make

# 返回项目根目录
cd "$CURRENT_DIR"

# 创建/usr/include/mymuduo目录，如果不存在
if [ ! -d "/usr/include/mymuduo" ]; then
    mkdir /usr/include/mymuduo
fi

# 将头文件拷贝到 /usr/include/mymuduo
for header in $(ls *.h)
do
    cp "$header" /usr/include/mymuduo
done

# 将so库拷贝到 /usr/lib
cp "$CURRENT_DIR/lib/libmymuduo.so" /usr/lib

# 更新动态链接库缓存
ldconfig