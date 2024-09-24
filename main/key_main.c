#include "Com/Com_config.h"
#include "Dir/Ketboard_Driver.h"
#include "Dir/Motor_Driver.h"
#include "Dir/Voice_Driver.h"
#include "Utils.h"
#include "Voice_Driver.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "freertos/idf_additions.h"
#include "hal/gpio_types.h"
#include "portmacro.h"
#include "string.h"
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include "Finger_Driver.h"
#include "Finger_Driver.h"
#include <string.h>


#define passworldnum 5 

static char passworld[passworldnum] = {7, 7, 7, 7, 0};

static char passbuff[passworldnum];


//指纹模块 标识
//是否要录入指纹 1 是  0不是
static uint8_t is_enroll_finger = 0 ; 
//是否有指纹放在上面
static uint8_t is_finger_on = 0 ; 

static QueueHandle_t key_Queue = NULL;
TaskHandle_t key_handler;
TaskHandle_t Finger_handler;
TaskHandle_t Verify_Finger_handler;


// 按键中断处理函数  参数是产生中断的GPIO引脚号
// IRAM_ATTR 宏定义表示回调函数在内存的某个特定的位置
static void IRAM_ATTR gpio_isr_key_handler(void *arg) {
  uint32_t gpio_num = (uint32_t)arg;
  //判断是按键还是手指触发的中断
  if(gpio_num == Finger_Int)
  {
    //手指触发的中断  要么需要验证  要么需要录入指纹
    //都需要关闭中断  参数 中断触发的引脚  
    is_finger_on = 1 ; 
    printf("指纹触发中断\r\n");
    gpio_isr_handler_remove(Finger_Int);
    gpio_isr_handler_remove(KEYBOARD_INT);
  } else {
    // 有按键触发   讲引脚号发送队列中
    // 发送队列时需要屏蔽中断
    xQueueSendFromISR(key_Queue, &gpio_num, NULL);
  }
}

// 处理按键中断函数任务
static void key_task(void *rig)
{
  uint32_t gpio_num;

  while (1) 
  {
    // 读取队中有没有按键值 如果有取出来  如果没有一直等待
    if (xQueueReceive(key_Queue, &gpio_num, GPIO_PORT_MAX)) {
      if (gpio_num == KEYBOARD_INT) {
        static uint8_t i = 0;
        // 读取按键值
        uint8_t key_num = KEYBOARD_ReadKey();
        printf("%d\r\n",key_num);
       //voice_Transmit_Data(key_num+1);
       //将按键值存入缓冲区中  
       passbuff[i++] = key_num;
      //如果是三个MMM 表示需要录入指纹
       if (i >= 3 && passbuff[i - 1] == 77 && 
          passbuff[i - 2] == 77 &&
          passbuff[i - 3] == 77) {
         // 需要录入指纹
         printf("需要录入指纹\r\n");
         i = 0;
         memset(passworld, 0, passworldnum);
         is_enroll_finger = 1;
       }
      }
    }
  vTaskDelay(10 / portTICK_PERIOD_MS);
  }

}

//注册指纹任务
static void Enroll_finger(void *reg)
{
  while (1) {
    if(is_enroll_finger)
    {
      //要注册指纹  --> 获取指纹模版数量
      uint8_t templatenum = Finger_GetTemperNumber();
      //录入指纹逻辑函数
      if(Finger_Sign_In(templatenum + 1 ))
      {
        //录入指纹失败
        printf("录入指纹失败\r\n");
      }
      printf("录入指纹成功\r\n");
      // 取消指纹验证
      Finger_Cancel();
      // 指纹休眠开启
      Finger_Sleep();
      is_enroll_finger = 0;
      is_finger_on = 0;

      // 开启中断
      gpio_isr_handler_add(KEYBOARD_INT, gpio_isr_key_handler,
                           (void *)KEYBOARD_INT);
      // 将指纹中断和中断回调函数关联起来
      gpio_isr_handler_add(Finger_Int, gpio_isr_key_handler,
                           (void *)Finger_Int);
    }
  DelayMs(200);
  }

}

//验证指纹任务
static void Verify_Finger_task(void * reg)
{
  while (1) {
    if(is_finger_on)
    {
      // 右手指在感应器上 开始验证指纹
      if (Finger_Verify()) {
        printf("指纹验证失败\r\n");
      }else {

        printf("指纹验证成功\r\n");
        // 开锁
        Motor_Openlock();
      }
      is_finger_on = 0;
      Finger_Cancel();
      Finger_Sleep();
      // 打开中断
      gpio_isr_handler_add(KEYBOARD_INT, gpio_isr_key_handler,
                           (void *)KEYBOARD_INT);
      // 将指纹中断和中断回调函数关联起来
      gpio_isr_handler_add(Finger_Int, gpio_isr_key_handler,
                           (void *)Finger_Int);
    }
  DelayMs(100);
  }
}



/* static void key_paddword_compare(void) {
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
} */
// 设置用户密码


void app_main(void) {
  // 按键先初始化
  KEYBOARD_Init();
  // 电机初始化
  Motor_Init();
  // 语音模块初始化
  voice_Init();
  // 指纹初始化
  Finger_Driver_Init();
  printf("发送芯片序列号\r\n");
  Finger_Driver_GetSerialNum();

  printf("设置加密登记\r\n");
  Finger_SetSecureLever();

  
  printf("获取模版数量\r\n");
   Finger_GetTemperNumber();
  printf(" 将录入指纹放进缓冲区中\r\n");
  Finger_EnrollImage();

  printf("生成特征\r\n");
  Finger_GenerateFeature(1);

  Finger_Cancel();
  Finger_Sleep();
  


  //验证
  // 创建一个队列,用来保存中断事件
  key_Queue = xQueueCreate(10, sizeof(uint32_t));
 
  // 启用中断
  // 启动之前先初始化中断服务例程确保硬件中断请求能正确映射到软件处理函数上
  // 参数控制中断行为的标志位 0表示系统随机分配一个非共享优先极为1\2\3 的中断
  // 当有连接次中断的硬件触发时会执行此中断
  gpio_install_isr_service(0);
  // 将硬件中断和软件处理中断连接起来
  // 参数1 触发中断的GPIO引脚
  // 参数2 触发中断时要调用的ISR处理函数
  // 参数3: 传递给ISR处理函数的参数  可以是任何的类型的数据
  gpio_isr_handler_add(KEYBOARD_INT, gpio_isr_key_handler, (void * )KEYBOARD_INT);
  //将指纹中断和中断回调函数关联起来
  gpio_isr_handler_add(Finger_Int, gpio_isr_key_handler, (void *)Finger_Int);

  // 将读取队列判断是否有中断发送数据到队列注册为一个任务
  xTaskCreate(key_task, 
  "Key_name", 
  2048,
   NULL, 10, 
   &key_handler);

   //创建注册指纹任务
  xTaskCreate(Enroll_finger,
   "Finger_name",
    2048,
     NULL,
      10,
       &Finger_handler);

  //创建指纹验证任务
       xTaskCreate(Verify_Finger_task,
   "Vetify_Finger_name",
    2048,
     NULL,
      10,
       &Verify_Finger_handler);
}

// 只有一个任务按键循环优先极10