#include "Motor_Driver.h"
#include "driver/gpio.h"
#include "Utils.h"
#include "hal/gpio_types.h"


void Motor_Init(void)
{
    //设置引脚模式  设置引脚初始化
    gpio_config_t io_conf ={};
    //设置输出/输出 
    io_conf.mode = GPIO_MODE_OUTPUT;
    //关闭中断
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask =((1ULL<<Motor_pin_0) |(1ULL << Motor_pin_1));
    gpio_config(&io_conf);

    //设置引脚初始电平
    gpio_set_level(Motor_pin_0,0);
    gpio_set_level(Motor_pin_1, 0 );
}
void Motor_Openlock(void)
{
    gpio_set_level(Motor_pin_0,1);
    gpio_set_level(Motor_pin_1, 0 );
    DelayMs(1000);

        gpio_set_level(Motor_pin_0,0);
    gpio_set_level(Motor_pin_1, 0 );
    DelayMs(1000);

        gpio_set_level(Motor_pin_0,0);
    gpio_set_level(Motor_pin_1, 1 );
    DelayMs(1000);

    gpio_set_level(Motor_pin_0,0);
    gpio_set_level(Motor_pin_1, 0 );
    DelayMs(1000);

}