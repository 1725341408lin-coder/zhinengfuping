#!/bin/bash

# 进入项目目录
cd /home/feng/xiaozhi/work/app4

echo "=========================================="
echo "开始清理旧的编译产物..."
echo "=========================================="

# 检查build目录是否存在
if [ -d "build" ]; then
    echo "1. 清理build目录中的编译中间文件..."
    cd build
    make clean 2>/dev/null
    
    echo "2. 删除CMake缓存配置文件..."
    rm -f CMakeCache.txt
    rm -f CMakeFiles/
    rm -rf CMakeFiles/
    rm -f cmake_install.cmake
    rm -f Makefile
    
    echo "3. 删除旧的app4可执行文件..."
    rm -f app4
    
    echo "4. 清理所有编译生成的.o文件..."
    find . -name "*.o" -delete
    
    echo "5. 清理编译日志文件..."
    rm -f CMakeOutput.log
    rm -f CMakeError.log
    
    cd ..
else
    echo "build目录不存在，将创建新目录..."
fi

echo ""
echo "=========================================="
echo "开始重新配置和编译..."
echo "=========================================="

# 创建build目录（如果不存在）
mkdir -p build
cd build

echo "6. 运行CMake配置（模拟器环境）..."
cmake -DSIMULATOR_LINUX=ON ..

if [ $? -ne 0 ]; then
    echo "错误: CMake配置失败！"
    exit 1
fi

echo ""
echo "7. 开始编译..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "错误: 编译失败！"
    exit 1
fi

echo ""
echo "=========================================="
echo "编译完成！"
echo "=========================================="

# 检查app4是否生成
if [ -f "app4" ]; then
    echo "✓ 新的app4可执行文件已成功生成"
    echo "  位置: /home/feng/xiaozhi/work/app4/build/app4"
    ls -lh app4
else
    echo "✗ 警告: app4可执行文件未找到"
    exit 1
fi

echo ""
echo "=========================================="
echo "所有操作完成！"
echo "=========================================="