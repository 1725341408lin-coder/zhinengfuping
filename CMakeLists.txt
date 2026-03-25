cmake_minimum_required(VERSION 3.15)

# 设置交叉编译器
set(CMAKE_C_COMPILER /home/feng/xiaozhi/work/download/toolchain-sunxi-glibc-gcc-830/toolchain/bin/arm-openwrt-linux-gcc)

# 设置交叉编译系统
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 工程名称app4
project(app4)



# 检查用户是否通过命令行传入了 SIMULATOR_LINUX
# 例如：cmake -DSIMULATOR_LINUX=ON ..
if(SIMULATOR_LINUX)
    add_compile_definitions(SIMULATOR_LINUX)  
endif()
# 拷贝res文件夹到build中，方便后续打包
file(COPY res DESTINATION ${PROJECT_BINARY_DIR})
#编译指定的lvgl、platform、font子目录
if(SIMULATOR_LINUX)
    add_subdirectory(${CMAKE_SOURCE_DIR}/../app_sdk/lvgl ${CMAKE_BINARY_DIR}/lvgl)
    add_subdirectory(${CMAKE_SOURCE_DIR}/../app_sdk/platform/x86linux ${CMAKE_BINARY_DIR}/platform)
    add_subdirectory(${CMAKE_SOURCE_DIR}/../app_sdk/component/font ${CMAKE_BINARY_DIR}/font)
    add_subdirectory(${CMAKE_SOURCE_DIR}/../app_sdk/component/osal ${CMAKE_BINARY_DIR}/osal)
    add_subdirectory(${CMAKE_SOURCE_DIR}/../app_sdk/component/wifi ${CMAKE_BINARY_DIR}/wifi)
else()
    add_subdirectory(${CMAKE_SOURCE_DIR}/../app_sdk/lvgl ${CMAKE_BINARY_DIR}/lvgl)
    add_subdirectory(${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113 ${CMAKE_BINARY_DIR}/platform)
    add_subdirectory(${CMAKE_SOURCE_DIR}/../app_sdk/component/font ${CMAKE_BINARY_DIR}/font)
    add_subdirectory(${CMAKE_SOURCE_DIR}/../app_sdk/component/osal ${CMAKE_BINARY_DIR}/osal)
    add_subdirectory(${CMAKE_SOURCE_DIR}/../app_sdk/component/wifi ${CMAKE_BINARY_DIR}/wifi)
endif()
#自动收集当前目录（.）下的所有源文件（.c、.cpp等），并将其存储在SRCS变量中
aux_source_directory(. SRCS)
# 生成名称为app4的可执行文件
add_executable(app4 ${SRCS})
if(SIMULATOR_LINUX)
#告诉gcc去查找头文件的路径
    target_include_directories(app4 PRIVATE
        ./
        ./res
        ./res/font
        ./res/image
        ${CMAKE_SOURCE_DIR}/../app_sdk/lvgl
        ${CMAKE_SOURCE_DIR}/../app_sdk/lvgl/demos
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/x86linux/src/porting
        ${CMAKE_SOURCE_DIR}/../app_sdk/component/font
        ${CMAKE_SOURCE_DIR}/../app_sdk/component/wifi
        ${CMAKE_SOURCE_DIR}/../app_sdk/component/osal
    )

#使弄出.o文件后可以链接到lvgl、font、libfreetype.a、z、SDL2、pthread、m、dl等库
    target_link_libraries(app4 PRIVATE
        lvgl
        font
        lvgl_porting
        wifi
        osal
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/x86linux/lib/libfreetype.a
        z
        SDL2
        pthread
        m
        dl
    )
else()
    target_include_directories(app4 PRIVATE
        ./
        ./res
        ./res/font
        ./res/image
        ${CMAKE_SOURCE_DIR}/../app_sdk/lvgl
        ${CMAKE_SOURCE_DIR}/../app_sdk/lvgl/demos
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/src/porting
        ${CMAKE_SOURCE_DIR}/../app_sdk/component/font
        ${CMAKE_SOURCE_DIR}/../app_sdk/component/wifi
        ${CMAKE_SOURCE_DIR}/../app_sdk/component/osal
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/include
    )

    target_link_libraries(app4 PRIVATE
        lvgl
        font
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/lib/libfreetype.so
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/lib/libwpa_client.so
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/lib/libuapi.so
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/lib/libz.so.1.2.11
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/lib/libcurl.so
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/lib/libcjson.a
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/lib/libssl.so
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/lib/libcrypto.so
        ${CMAKE_SOURCE_DIR}/../app_sdk/platform/t113/lib/libnghttp2.so
        lvgl_porting
        wifi
        osal
        pthread
        m
        dl
        rt
    )
endif()
# 当使用 PRIVATE 关键字时，表示所链接的库仅用于当前目标的实现，而不会传递给依赖该目标的其他目标。
# 换句话说：
# 当前目标（app4）会链接这些库并使用它们。
# 其他依赖 app4的目标（比如另一个库或可执行文件）不会自动链接这些库。