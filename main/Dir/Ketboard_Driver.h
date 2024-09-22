#ifndef __KETBOARD_DRIVER_H
#define __KETBOARD_DRIVER_H
/* 
����ʹ�� 
#pragma once  ��gcc�����±�ʾ�ļ�ֻ����һ��
 */

#include "driver/gpio.h"

#include "Utils.h"

/// SCLʱ������ P1
#define I2C_SCL GPIO_NUM_1
/// SDA�������� P2
#define I2C_SDA GPIO_NUM_2
/// INT�ж����� P0
#define KEYBOARD_INT GPIO_NUM_0

/// ����SDA����Ϊ���뷽��
#define I2C_SDA_IN gpio_set_direction(I2C_SDA, GPIO_MODE_INPUT)
/// ����SDA����Ϊ�������
#define I2C_SDA_OUT gpio_set_direction(I2C_SDA, GPIO_MODE_OUTPUT)

/// ����SCL����
#define I2C_SCL_H gpio_set_level(I2C_SCL, 1)
/// ����SCL����
#define I2C_SCL_L gpio_set_level(I2C_SCL, 0)

/// ����SDA����
#define I2C_SDA_H gpio_set_level(I2C_SDA, 1)
/// ����SDA����
#define I2C_SDA_L gpio_set_level(I2C_SDA, 0)

/// ��ȡSDA���ŵ�ƽ��ֵ
#define I2C_READ_SDA gpio_get_level(I2C_SDA)

/// SC12B�ĵ�ַ
#define I2C_ADDR 0x42

void KEYBOARD_Init(void);
uint8_t KEYBOARD_ReadKey(void);


#endif 