# 指定cmake的最低版本
cmake_minimum_required(VERSION 3.16)
# 在STM32开发中，需要将HAL库和FreeRTOS相关代码拷贝到项目文件夹中
# 下面一行将esp-idf的源码中的头文件和c代码包含进来
# `$ENV{IDF_PATH}` 是 esp-idf 的绝对路径
# 所以不需要将esp-idf的代码拷贝到项目文件夹中
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# 给编译好的二进制文件起一个名字，`hello_world.bin`
project(key_example)