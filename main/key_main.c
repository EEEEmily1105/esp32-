#include "Com/Com_config.h"
#include "Dir/Ketboard_Driver.h"
#include "Dir/Motor_Driver.h"
#include "Dir/Voice_Driver.h"
#include "Utils.h"
#include "Voice_Driver.h"
#include "assert.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "freertos/idf_additions.h"
#include "hal/gpio_types.h"
#include "portmacro.h"
#include "string.h"
#include "sys/unistd.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/_intsup.h>
#include <sys/errno.h>

#define passworldnum 5 

static char passworld[passworldnum] = {7, 7, 7, 7, 0};
static uint8_t setworld = 1;
static char passbuff[passworldnum];
static uint8_t key = 0;
// static uint8_t flag = 1 ;

static QueueHandle_t key_Queue = NULL;
TaskHandle_t key_handler;
// 按键密码匹配函数
static void key_paddword_compare(void);

// 按键中断处理函数  参数是产生中断的GPIO引脚号
// IRAM_ATTR 宏定义表示回调函数在内存的某个特定的位置
static void IRAM_ATTR gpio_isr_key_handler(void *arg) {
  uint32_t gpio_num = (uint32_t)arg;
  // 有按键触发   讲引脚号发送队列中
  // 发送队列时需要屏蔽中断
  xQueueSendFromISR(key_Queue, &gpio_num, NULL);
}

// 处理按键中断函数任务
static void key_task(void *rig) {
  uint32_t gpio_num;

  while (1) 
  {

    // 读取队中有没有按键值 如果有取出来  如果没有一直等待
    if (xQueueReceive(key_Queue, &gpio_num, GPIO_PORT_MAX)) {
      if (gpio_num == KEYBOARD_INT) {
        static uint8_t i = 0;
        // 读取按键值
        uint8_t key_num = KEYBOARD_ReadKey();
        // 可以写一个状态机
        switch (key_num) 
        {
        case END: // 匹配密码
        {
          // 匹配按键数组和密码是否匹配
          key_paddword_compare();
          // 清楚passbuff数组
          memset(passbuff, 0, strlen(passbuff));
          i = 0;
          break;
        }

        case BEGIN: {
          // 设置密码

          break;
        }
        default: // 将密码放进缓冲区中  表示正在输入密码
          printf("press key :%d\r\n", key_num);

          // 只有播报时候数字+1 放进数组中时不用+1
          voice_Transmit_Data(key_num + 1);
          // 将按键值放进数组中
          passbuff[i++] = key_num;
        }
      }
    }
  vTaskDelay(10 / portTICK_PERIOD_MS);
  }

}

static void key_paddword_compare(void) {
  // 判断数组中的数和按键密码是否一样
  int8_t num = strlen(passworld);
  // printf("NUM = %d\r\n", num);
  for (uint8_t j = 0; j < num; j++) {

    if (passworld[j] != passbuff[j]) {
      // 当有发现不一样时重新开始按键

      voice_Transmit_Data(65);
      DelayMs(2);
      voice_Transmit_Data(69);
     
      return;
    }
    // 一样的密码
    // 开锁
    voice_Transmit_Data(25);
    Motor_Openlock();
    return;
  }
}
// 设置用户密码


void app_main(void) {
  // 按键先初始化
  KEYBOARD_Init();
  // 电机初始化
  Motor_Init();
  // 语音模块初始化
  voice_Init();
  // 发送一个数据

  // 创建一个队列,用来保存中断事件
  key_Queue = xQueueCreate(10, sizeof(uint32_t));
  // 将读取队列判断是否有中断发送数据到队列注册为一个任务
  xTaskCreate(key_task, "Key_name", 2048, NULL, 10, &key_handler);
  // 启用中断
  // 启动之前先初始化中断服务例程确保硬件中断请求能正确映射到软件处理函数上
  // 参数控制中断行为的标志位 0表示系统随机分配一个非共享优先极为1\2\3 的中断
  // 当有连接次中断的硬件触发时会执行此中断
  gpio_install_isr_service(0);
  // 将硬件中断和软件处理中断连接起来
  // 参数1 触发中断的GPIO引脚
  // 参数2 触发中断时要调用的ISR处理函数
  // 参数3: 传递给ISR处理函数的参数  可以是任何的类型的数据
  gpio_isr_handler_add(KEYBOARD_INT, gpio_isr_key_handler, KEYBOARD_INT);
}

// 只有一个任务按键循环优先极10