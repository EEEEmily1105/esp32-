#include "Motor_Driver.h"
#include "driver/gpio.h"
#include "Utils.h"
#include "hal/gpio_types.h"


void Motor_Init(void)
{
    //��������ģʽ  �������ų�ʼ��
    gpio_config_t io_conf ={};
    //�������/��� 
    io_conf.mode = GPIO_MODE_OUTPUT;
    //�ر��ж�
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask =((1ULL<<Motor_pin_0) |(1ULL << Motor_pin_1));
    gpio_config(&io_conf);

    //�������ų�ʼ��ƽ
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