#ifndef __KETBOARD_DRIVER_H
#define __KETBOARD_DRIVER_H
/* 
或者使用 
#pragma once  在gcc编译下表示文件只引用一次
 */

#include "driver/gpio.h"

#include "Utils.h"

/// SCL时钟引脚 P1
#define I2C_SCL GPIO_NUM_1
/// SDA数据引脚 P2
#define I2C_SDA GPIO_NUM_2
/// INT中断引脚 P0
#define KEYBOARD_INT GPIO_NUM_0

/// 设置SDA引脚为输入方向
#define I2C_SDA_IN gpio_set_direction(I2C_SDA, GPIO_MODE_INPUT)
/// 设置SDA引脚为输出方向
#define I2C_SDA_OUT gpio_set_direction(I2C_SDA, GPIO_MODE_OUTPUT)

/// 拉高SCL引脚
#define I2C_SCL_H gpio_set_level(I2C_SCL, 1)
/// 拉低SCL引脚
#define I2C_SCL_L gpio_set_level(I2C_SCL, 0)

/// 拉高SDA引脚
#define I2C_SDA_H gpio_set_level(I2C_SDA, 1)
/// 拉低SDA引脚
#define I2C_SDA_L gpio_set_level(I2C_SDA, 0)

/// 读取SDA引脚电平的值
#define I2C_READ_SDA gpio_get_level(I2C_SDA)

/// SC12B的地址
#define I2C_ADDR 0x42

void KEYBOARD_Init(void);
uint8_t KEYBOARD_ReadKey(void);


#endif 